/// bin2json.cpp
/// Contains the implementations for the associated functions that convert from json to bin

#include "commonutils.h"
#include "datainfos.h"
#include "custominfos.h"
#include "bin2json.h"

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <nlohmann/json.hpp>


void bin2json::convertMapgridBinToJson(const std::string &file_name, Version version) {
    // Deserialize bin
    std::filesystem::path file_path = file_name;
    verifyBinFileExtension(file_path);

    std::vector<std::byte> buffer = readBinFileIntoBuffer(file_path);
    if (buffer.empty()) {
        fprintf(stderr, "Warning - Issue reading file: %s\n", file_name.c_str());
        return;
    }

    // Process
    const MapGridInfo& info = version == Version::Custom ? CustomMapGridInfo : MapGridInfoAll;
    nlohmann::ordered_json json = bin2json::parseMapgridBytes(info, buffer);

    // Serialize as json
    file_path.replace_extension("json");
    std::ofstream output_json_file(file_path);
    output_json_file << json.dump(2);    
}

nlohmann::ordered_json bin2json::parseMapgridBytes(const MapGridInfo& info, const std::vector<std::byte>& buffer) {
    nlohmann::ordered_json json;

    json["mapgridSizeInBits"] = sizeInBits(info.mapgrid_masks.begin()->second);
    json["mapgridMasks"] = nlohmann::ordered_json::object();
    for (auto masks: info.mapgrid_masks) {
        json["mapgridMasks"][masks.first] = intToHexStd(masks.second);
    }

    json["mapgrid"] = nlohmann::ordered_json::array();

    uint16_t temp_var{0};
    unsigned bytes_loaded{0};
    const unsigned bytes_needed{sizeInBits(temp_var) / 8};
    for (auto byte: buffer) {
        temp_var = temp_var >> 8 | static_cast<uint16_t>(byte) << 8;
        bytes_loaded++;

        if (bytes_loaded == bytes_needed) {
            json["mapgrid"].push_back(deserializeSingleMapgridTile(info.mapgrid_masks, temp_var));

            // Reset
            bytes_loaded = 0;
            temp_var = 0;
        }
    }

    return json;
}

nlohmann::ordered_json bin2json::deserializeSingleMapgridTile(const std::map<std::string, uint16_t>& mask, uint16_t value){
    nlohmann::ordered_json mapgrid = nlohmann::ordered_json::object();
    for (auto mask: mask) {
        // Mask the full variable to isolate the var we want, then bit shift the mask until we hit the
        //  start of our mask
        uint16_t val = (value & mask.second) >> firstBitOffset(mask.second);
        mapgrid[mask.first] = val;
    }
    return mapgrid;
}

void bin2json::convertMetatilesBinToJson(const std::string &file_name, Version version) {
    // Deserialize bin
    std::filesystem::path file_path = file_name;
    verifyBinFileExtension(file_path);

    std::vector<std::byte> buffer = readBinFileIntoBuffer(file_path);
    if (buffer.empty()) {
        fprintf(stderr, "Warning - Issue reading file: %s\n", file_name.c_str());
        return;
    }

    // Process
    const MetatilesInfo& info = version == Version::Custom ? CustomMetatilesInfo :
                                version == Version::FireRedLeafGreen ? MetatilesInfoFRLG :
                                                           MetatilesInfoRSE;
    bool is_secondary = filepathContainsSecondary(file_path);
    nlohmann::ordered_json json = bin2json::parseMetatilesBytes(info, is_secondary, buffer);

    // Serialize as json
    file_path.replace_extension("json");
    std::ofstream output_json_file(file_path);
    output_json_file << json.dump(2);    
}

nlohmann::ordered_json bin2json::parseMetatilesBytes(const MetatilesInfo& info, bool is_secondary, const std::vector<std::byte>& buffer) {
    nlohmann::ordered_json json;
    
    json["numMetatiles"] = is_secondary ? info.num_metatiles_in_secondary : info.num_metatiles_in_primary;
    json["numTiles"] = is_secondary ? info.num_tiles_in_secondary : info.num_tiles_in_primary;
    json["numPals"] = is_secondary ? info.num_pals_in_secondary : info.num_pals_in_primary;
    json["numTilesInMetatile"] = info.num_tiles_in_metatile;

    json["tilesMasks"] = nlohmann::ordered_json::object();
    for (auto masks: info.tiles_masks) {
        json["tilesMasks"][masks.first] = intToHexStd(masks.second);
    }

    json["metatiles"] = nlohmann::ordered_json::array();

    uint16_t temp_var{0};
    unsigned bytes_loaded{0};
    const unsigned bytes_needed{sizeInBits(temp_var) / 8};
    std::vector<uint16_t> tiles_array;
    for (auto byte: buffer) {
        // Pack bytes little-endian
        temp_var = temp_var >> 8 | static_cast<uint16_t>(byte) << 8;
        bytes_loaded++;

        // Each iteration we load up one byte until we have enough bites for the size of our tile, then we load that one tile into the array
        if (bytes_loaded == bytes_needed) {
            tiles_array.push_back(temp_var);
            
            // Reset
            bytes_loaded = 0;
            temp_var = 0;
        }
        // Once we have the number of tiles per metatile we can jump right into creatting a fully formed metatile and the correct number of tiles
        if (tiles_array.size() == info.num_tiles_in_metatile) {
            nlohmann::ordered_json tiles = deserializeSingleMetatile(info, tiles_array);
            // After filling tiles for that metatile, we add it to the list and clear it
            json["metatiles"].push_back(tiles);
            tiles_array.clear();
        }

    }

    return json;
}

nlohmann::ordered_json bin2json::deserializeSingleMetatile(const MetatilesInfo& info, std::vector<uint16_t> tiles_array) {
    nlohmann::ordered_json tiles_json = nlohmann::ordered_json::object();
    tiles_json["tiles"] = nlohmann::ordered_json::array();

    for (auto tileInt : tiles_array) {
        nlohmann::ordered_json tile = deserializeSingleTile(info, tileInt);
        tiles_json["tiles"].push_back(tile);
    }
    return tiles_json;
}

nlohmann::ordered_json bin2json::deserializeSingleTile(const MetatilesInfo& info, uint16_t tile) {
    nlohmann::ordered_json tile_json = nlohmann::ordered_json::object();
    for (auto mask: info.tiles_masks) {
        // Mask the full variable to isolate the var we want, then bit shift the mask until we hit the
        //  start of our mask
        uint16_t val = (tile & mask.second) >> firstBitOffset(mask.second);
        tile_json[mask.first] = val;
    }
    return tile_json;
}

void bin2json::convertMetatileAttributesBinToJson(const std::string &file_name, Version version) {
    // Deserialize bin
    std::filesystem::path file_path = file_name;
    verifyBinFileExtension(file_path);

    std::vector<std::byte> buffer = readBinFileIntoBuffer(file_path);
    if (buffer.empty()) {
        fprintf(stderr, "Warning - Issue reading file: %s\n", file_name.c_str());
        return;
    }

    // Process
    bool is_secondary = filepathContainsSecondary(file_path);
    nlohmann::ordered_json json;
    if (version == Version::RubySapphireEmerald) {
        json = bin2json::parseMetatileAttributesBytes(MetatileAttributesInfoRSE, is_secondary, buffer);
    } else if (version == Version::FireRedLeafGreen) {
        json = bin2json::parseMetatileAttributesBytes(MetatileAttributesInfoFRLG, is_secondary, buffer);
    } else if (version == Version::Custom) {
        json = bin2json::parseMetatileAttributesBytes(CustomMetatileAttributesInfo, is_secondary, buffer);
    } else {
        fprintf(stderr, "Warning - Unknown version for metatile attributes: %s\n", file_name.c_str());
        return;
    }

    // Serialize as json
    file_path.replace_extension("json");
    std::ofstream output_json_file(file_path);
    output_json_file << json.dump(2);
}

template<class T>
nlohmann::ordered_json bin2json::parseMetatileAttributesBytes(const MetatileAttributesInfo<T>& info, bool is_secondary, const std::vector<std::byte>& buffer) {
    nlohmann::ordered_json json;

    json["numMetatiles"] = is_secondary ? info.num_metatiles_in_secondary : info.num_metatiles_in_primary;
    json["attributeSizeInBits"] = sizeInBits(T{});

    json["attributeMasks"] = nlohmann::ordered_json::object();
    for (auto masks: info.attribute_masks) {
        json["attributeMasks"][masks.first] = intToHexStd(masks.second);
    }

    json["metatiles"] = nlohmann::ordered_json::array();

    T temp_var{0};
    unsigned bytes_loaded{0};
    unsigned bytes_needed{sizeInBits(temp_var) / 8};
    for (auto byte: buffer) {
        // Pack bytes little-endian
        temp_var = temp_var >> 8 | (static_cast<T>(byte) << (sizeInBits(temp_var) - 8));
        bytes_loaded++;

        if (bytes_loaded == bytes_needed) {
            nlohmann::ordered_json attributes = deserializeSingleMetatileAttribute(info, temp_var);
            json["metatiles"].push_back(attributes);

            // Reset
            bytes_loaded = 0;
            temp_var = 0;
        }
    }

    return json;
}

template<class T>
nlohmann::ordered_json bin2json::deserializeSingleMetatileAttribute(const MetatileAttributesInfo<T>& info, T attr) {
    nlohmann::ordered_json attribute_json = nlohmann::ordered_json::object();
    for (auto mask: info.attribute_masks) {
        // Mask the full variable to isolate the var we want, then bit shift the mask until we hit the
        //  start of our mask
        T val = (attr & mask.second) >> firstBitOffset(mask.second);
        attribute_json["attributes"][mask.first] = val;
    }
   return attribute_json;
}
