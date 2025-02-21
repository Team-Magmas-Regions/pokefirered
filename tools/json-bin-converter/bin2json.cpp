/// bin2json takes in a version followed by a usage mode of either mapgrid, metatiles, or metatile_attributes,
///     followed by a list of bin files. The json files will be output to the same paths as passed in except
///     the extension will change. The mode to use are detailed below:
/// mapgrid:             used for data/layouts/*/border.bin and data/layouts/*/map.bin
/// metatiles:           used for data/tilesets/*/*/metatiles.bin
/// metatile_attributes: used for data/tilesets/*/*/metatile_attributes.bin
///
/// The version will determine how to pack the data. For the version the options are rse, frlg, custom
///   For the custom "version" you will have to edit this code to add your own packer which you can find 
///   in the custominfos.h file. I'd suggest committing this to your project :)

#include "jsonbinconverter.h"
#include "commonutils.h"
#include "datainfos.h"
#include "custominfos.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>

constexpr auto Usage{"USAGE: bin2json <version: rse|frlg> <mode: mapgrid|metatiles|metatile_attributes> ...\n"};

enum class UsageMode : unsigned {
    Mapgrid,
    Metatiles,
    MetatileAttributes,

    Error
};
constexpr auto NumModes = static_cast<unsigned>(UsageMode::Error);
const char* UsageModeStr[NumModes] = {"mapgrid", "metatiles", "metatile_attributes"};
UsageMode StrToMode(const char* str) {
    for(unsigned i = 0; i < NumModes; i++) {
        if (strcmp(str, UsageModeStr[i]) == 0) {
            return static_cast<UsageMode>(i);
        }
    }
    return UsageMode::Error;
}

enum class Version : unsigned {
    RubySapphireEmerald,
    FireRedLeafGreen,
    Custom,

    Error
};
constexpr auto NumVersions = static_cast<unsigned>(Version::Error);
const char* VersionStr[NumVersions] = {"rse", "frlg", "custom"};
Version StrToVersion(const char* str) {
    for(unsigned i = 0; i < NumVersions; i++) {
        if (strcmp(str, VersionStr[i]) == 0) {
            return static_cast<Version>(i);
        }
    }
    return Version::Error;
}

nlohmann::ordered_json convertMapgridBinToJson(Version version, const std::vector<std::byte>& buffer) {
    nlohmann::ordered_json json;
    const MapGridInfo& info = version == Version::Custom ? CustomMapGridInfo : MapGridInfoAll;

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
            nlohmann::ordered_json mapgrid = nlohmann::ordered_json::object();
            for (auto mask: info.mapgrid_masks) {
                // Mask the full variable to isolate the var we want, then bit shift the mask until we hit the
                //  start of our mask
                uint16_t val = (temp_var & mask.second) >> firstBitOffset(mask.second);
                mapgrid[mask.first] = val;
            }
            json["mapgrid"].push_back(mapgrid);

            // Reset
            bytes_loaded = 0;
            temp_var = 0;
        }
    }

    return json;
}

nlohmann::ordered_json convertMetatilesBinToJson(Version version, bool is_secondary, const std::vector<std::byte>& buffer) {
    nlohmann::ordered_json json;
    const MetatilesInfo& info = version == Version::Custom           ? CustomMetatilesInfo : 
                                version == Version::FireRedLeafGreen ? MetatilesInfoFRLG : 
                                                                       MetatilesInfoRSE;
    
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
    nlohmann::ordered_json tiles = nlohmann::ordered_json::object();
    tiles["tiles"] = nlohmann::ordered_json::array();
    for (auto byte: buffer) {
        temp_var = temp_var >> 8 | static_cast<uint16_t>(byte) << 8;
        bytes_loaded++;

        if (bytes_loaded == bytes_needed) {
            nlohmann::ordered_json tile = nlohmann::ordered_json::object();
            for (auto mask: info.tiles_masks) {
                // Mask the full variable to isolate the var we want, then bit shift the mask until we hit the
                //  start of our mask
                uint16_t val = (temp_var & mask.second) >> firstBitOffset(mask.second);
                tile[mask.first] = val;
            }
            tiles["tiles"].push_back(tile);

            // After filling tiles for that metatile, we add it to the list and clear it
            if (tiles["tiles"].size() == info.num_tiles_in_metatile) {
                json["metatiles"].push_back(tiles);
                tiles["tiles"].clear();
            }

            // Reset
            bytes_loaded = 0;
            temp_var = 0;
        }
    }

    return json;
}

template<class T>
nlohmann::ordered_json convertMetatileAttributesBinToJson(const MetatileAttributesInfo<T>& info, bool is_secondary, const std::vector<std::byte>& buffer) {
    nlohmann::ordered_json json;

    json["numMetatiles"] = is_secondary ? info.num_metatiles_in_secondary : info.num_metatiles_in_primary;
    json["attributeSizeInBits"] = sizeInBits(info.attribute_masks);

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
            nlohmann::ordered_json attributes = nlohmann::ordered_json::object();
            for (auto mask: info.attribute_masks) {
                // Mask the full variable to isolate the var we want, then bit shift the mask until we hit the
                //  start of our mask
                T val = (temp_var & mask.second) >> firstBitOffset(mask.second);
                attributes["attributes"][mask.first] = val;
            }
            json["metatiles"].push_back(attributes);

            // Reset
            bytes_loaded = 0;
            temp_var = 0;
        }
    }

    return json;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        FATAL_ERROR(Usage);
    }

    Version version = StrToVersion(argv[1]);
    if (version == Version::Error) {
        FATAL_ERROR(Usage);
    }
    
    UsageMode mode = StrToMode(argv[2]);
    if (mode == UsageMode::Error) {
        FATAL_ERROR(Usage);
    }

    std::vector<std::string> file_names;
    for (int i = 3; i < argc; i++) {
        file_names.emplace_back(argv[i]);
    }

    // If we're using metatiles or metatile attributes, we determine the secondary by if it's found in the path
    bool is_secondary{false};

    for(auto file_name: file_names) {        
        // Deserialize bin
        std::filesystem::path file_path = file_name;
        std::vector<std::byte> buffer = readBinFileIntoBuffer(file_path);
        if (buffer.empty()) {
            std::cout << "Warning - Issue reading file:" << file_path << std::endl;
            break;
        }

        // Process
        nlohmann::ordered_json json;
        switch(mode) {
            case UsageMode::Mapgrid:
                json = convertMapgridBinToJson(version, buffer);
                break;
            case UsageMode::Metatiles:
                is_secondary = filepathContainsSecondary(file_path);
                json = convertMetatilesBinToJson(version, is_secondary, buffer);
                break;
            case UsageMode::MetatileAttributes:
                is_secondary = filepathContainsSecondary(file_path);
                if (version == Version::RubySapphireEmerald) {
                    json = convertMetatileAttributesBinToJson(MetatileAttributesInfoRSE, is_secondary, buffer);
                } else if (version == Version::FireRedLeafGreen) {
                    json = convertMetatileAttributesBinToJson<>(MetatileAttributesInfoFRLG, is_secondary, buffer);
                } else if (version == Version::Custom) {
                    json = convertMetatileAttributesBinToJson(CustomMetatileAttributesInfo, is_secondary, buffer);
                } else {
                    FATAL_ERROR(Usage);
                }
                break;
            case UsageMode::Error:
                FATAL_ERROR(Usage);
                break;
        }

        // Serialize as json
        file_path.replace_extension("json");
        std::ofstream output_json_file(file_path);
        output_json_file << json.dump(2);
    }

    return 0;
}
