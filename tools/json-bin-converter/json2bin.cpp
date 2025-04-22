/// json2bin.cpp
/// Contains the implementations for the associated functions that convert from json to binary

#include "jsonbinconverter.h"
#include "json2bin.h"
#include "commonutils.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

void json2bin::convertMapgridJsonToBin(const std::string& file_name) {
    // Deserialize json
    std::filesystem::path path{file_name};
    verifyJsonFileExtension(file_name);
    std::ifstream json_stream(path);
    nlohmann::ordered_json mapgrid_json = nlohmann::json::parse(json_stream);

    // Parse the mapgrid data from the json into our expected format
    std::vector<uint16_t> mapgrid_tiles = json2bin::parseMapgridJson(mapgrid_json);

    // Setup file to write to
    path.replace_extension("bin");
    std::ofstream bin_file{path, std::ios::binary | std::ios::out};

    // Serialize our bin file
    for (const uint16_t& tile: mapgrid_tiles) {
        bin_file.write(reinterpret_cast<const char *>(&tile), sizeof(tile));
    }
}

std::vector<uint16_t> json2bin::parseMapgridJson (const nlohmann::ordered_json& mapgrid_json) {
    // Get each of the masks from which the key should be the name of the mask that you'll find in the
    //  mapgrid data, and a string containing a hexidecimal number we can convert to an int to mask against 
    std::map<std::string, uint16_t> masks;
    for (const auto& [key, value]: mapgrid_json["mapgridMasks"].items()) {
        masks.insert(std::make_pair(key, hexToInt<uint16_t>(value.template get<std::string>())));
    }

    std::vector<nlohmann::ordered_json> mapgrid = mapgrid_json["mapgrid"];
    std::vector<uint16_t> mapgrid_tiles;

    // Iterate through each of the masks that each element of the mapgrid data should have, the data then
    //  gets bit shifted to the start of the mask, and then finally masked to make sure we don't overflow
    //  into another masked section
    for (auto tile_data: mapgrid) {
        uint16_t temp_tile{0};
        for (const auto& [key, value]: masks) {
            temp_tile |= static_cast<uint16_t>(tile_data[key].template get<uint16_t>() << firstBitOffset(value) & value);
        }
        mapgrid_tiles.push_back(temp_tile);
    }

    return mapgrid_tiles;
}

void json2bin::convertMetatilesJsonToBin(const std::string& file_name) {
    // Deserialize json
    std::filesystem::path path{file_name};
    verifyJsonFileExtension(file_name);

    std::ifstream json_stream(path);
    nlohmann::ordered_json metatiles_json = nlohmann::json::parse(json_stream);

    std::vector<uint16_t> tiles = json2bin::parseMetatilesJson(metatiles_json, path);

    // Setup file to write to
    path.replace_extension("bin");
    std::ofstream bin_file{path, std::ios::binary | std::ios::out};
    
    // Serialize our bin file
    for (const uint16_t& tile: tiles) {
        bin_file.write(reinterpret_cast<const char *>(&tile), sizeof(tile));
    }
}

std::vector<uint16_t> json2bin::parseMetatilesJson(nlohmann::ordered_json metatiles_json, std::optional<std::filesystem::path> path) {
    // Do some error checking
    if (metatiles_json["metatiles"].size() > metatiles_json["numMetatiles"]) {
        fprintf(stderr, "Warning: More metatiles than in numMetatiles for file %s.\n", path ? path->c_str() : "");
    }
    if (metatiles_json["metatiles"].size() > metatiles_json["numTiles"]) {
        fprintf(stderr, "Warning: More tiles than in numTiles for file %s.\n", path ? path->c_str() : "");
    }

    // Get each of the masks from which the key should be the name of the mask that you'll find in the
    //  tiles data, and a string containing a hexidecimal number we can convert to an int to mask against 
    std::map<std::string, uint16_t> masks;
    for (const auto& [key, value]: metatiles_json["tilesMasks"].items()) {
        masks.insert(std::make_pair(key, hexToInt<uint16_t>(value.template get<std::string>())));
    }

    std::vector<nlohmann::ordered_json> metatiles_arr = metatiles_json["metatiles"];
    std::vector<uint16_t> tiles;

    // Iterate through each of the masks that each element of the tiles data should have, the data then
    //  gets bit shifted to the start of the mask, and then finally masked to make sure we don't overflow
    //  into another masked section
    for (auto metatile: metatiles_arr) {
        if (metatile["tiles"].size() != metatiles_json["numTilesInMetatile"]) {
            fprintf(stderr, "Warning: Num tiles in metatile array differ to numTilesInMetatile for file %s.\n", path ? path->c_str() : "");
        }
        for (auto tile_data: metatile["tiles"]) {
            uint16_t temp_tile{0};
            for (const auto& [key, value]: masks) {
                temp_tile |= static_cast<uint16_t>(tile_data[key].template get<uint16_t>() << firstBitOffset(value) & value);
            }
            tiles.push_back(temp_tile);
        }
    }

    return tiles;
}

void json2bin::convertMetatileAttributesJsonToBin(const std::string& file_name) {
    // Deserialize json
    std::filesystem::path path{file_name};
    verifyJsonFileExtension(file_name);

    std::ifstream json_stream(path);
    nlohmann::ordered_json metatile_attribute_json = nlohmann::json::parse(json_stream);

    // Setup file to write to
    path.replace_extension("bin");
    std::ofstream bin_file{path, std::ios::binary | std::ios::out};

    // Since the size can vary, we template this function to handle various attribute sizes
    unsigned attribute_size_in_bits = {0};
    attribute_size_in_bits = metatile_attribute_json["attributeSizeInBits"].template get<unsigned>();
    if (attribute_size_in_bits == 8) {
        std::vector<uint8_t> metatile_attributes_data = json2bin::parseMetatileAttributesJson<uint8_t>(metatile_attribute_json, path);

        // Serialize our bin file
        for (const uint8_t& metatile_attr: metatile_attributes_data) {
            bin_file.write(reinterpret_cast<const char *>(&metatile_attr), sizeof(metatile_attr));
        }
    } else if (attribute_size_in_bits == 16) {
        std::vector<uint16_t> metatile_attributes_data = json2bin::parseMetatileAttributesJson<uint16_t>(metatile_attribute_json, path);

        // Serialize our bin file
        for (const uint16_t& metatile_attr: metatile_attributes_data) {
            bin_file.write(reinterpret_cast<const char *>(&metatile_attr), sizeof(metatile_attr));
        }
    } else if (attribute_size_in_bits == 32) {
        std::vector<uint32_t> metatile_attributes_data = json2bin::parseMetatileAttributesJson<uint32_t>(metatile_attribute_json, path);

        // Serialize our bin file
        for (const uint32_t& metatile_attr: metatile_attributes_data) {
            bin_file.write(reinterpret_cast<const char *>(&metatile_attr), sizeof(metatile_attr));
        }
    }  else if (attribute_size_in_bits == 64) { // If someone actually uses this you're insane lol
        std::vector<uint64_t> metatile_attributes_data = json2bin::parseMetatileAttributesJson<uint64_t>(metatile_attribute_json, path);

        // Serialize our bin file
        for (const uint64_t& metatile_attr: metatile_attributes_data) {
            bin_file.write(reinterpret_cast<const char *>(&metatile_attr), sizeof(metatile_attr));
        }
    } else {
        fprintf(stderr, "Warning: attributeSizeInBits either missing or not fixed width 8, 16, 32, 64\n");
    }
}

template <typename T>
std::vector<T> json2bin::parseMetatileAttributesJson(nlohmann::ordered_json& metatile_attribute_json, std::optional<std::filesystem::path> path) {
    if (metatile_attribute_json["metatiles"].size() > metatile_attribute_json["numMetatiles"]) {
        fprintf(stderr, "Warning: More metatiles than in numMetatiles in file %s.\n", path ? path->c_str() : "");
    }

    // Get each of the masks from which the key should be the name of the mask that you'll find in the
    //  tiles data, and a string containing a hexidecimal number we can convert to an int to mask against 
    std::map<std::string, T> masks;
    for (const auto& [key, value]: metatile_attribute_json["attributeMasks"].items()) {
        masks.insert(std::make_pair(key, hexToInt<T>(value.template get<std::string>())));
    }

    std::vector<T> metatile_attributes_data;

    // Iterate through each of the masks that each element of the tiles data should have, the data then
    //  gets bit shifted to the start of the mask, and then finally masked to make sure we don't overflow
    //  into another masked section
    for (auto metatile: metatile_attribute_json["metatiles"]) {
        T temp_metatile_attributes{0};
        for (const auto& [key, value]: masks) {
            temp_metatile_attributes |= static_cast<T>(metatile["attributes"][key].template get<T>() << firstBitOffset(value) & value);
        }
        metatile_attributes_data.push_back(temp_metatile_attributes);
    }

    return metatile_attributes_data;
}
