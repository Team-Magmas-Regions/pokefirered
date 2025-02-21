// json2bin.cpp
/// json2bin takes in a usage mode of either mapgrid, metatiles, or metatile_attributes and then a list of 
///     json files. The bin files will be output to the same paths as passed in except the extension will 
///     change. The mode to use are detailed below:
/// mapgrid:             used for data/layouts/*/border.json and data/layouts/*/map.json
/// metatiles:           used for data/tilesets/*/*/metatiles.json
/// metatile_attributes: used for data/tilesets/*/*/metatile_attributes.json 

#include "jsonbinconverter.h"
#include "commonutils.h"

#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

constexpr auto Usage{"USAGE: json2bin <mapgrid|metatiles|metatile_attributes> ...\n"};

enum class UsageMode : unsigned{
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

void convertMapgridJsonToBin(const std::vector<std::string>& file_names) {
    for(auto file_name: file_names) {
        // Deserialize json
        std::filesystem::path path{file_name};
        std::ifstream json_stream(path);
        nlohmann::ordered_json mapgrid_json = nlohmann::json::parse(json_stream);

        // Setup file to write to
        path.replace_extension("bin");
        std::ofstream bin_file{path, std::ios::binary | std::ios::out};

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

        // Write our bin file
        for (const uint16_t& tile: mapgrid_tiles) {
            bin_file.write(reinterpret_cast<const char *>(&tile), sizeof(tile));
        }
    }
}

void convertMetatilesJsonToBin(const std::vector<std::string>& file_names) {
    for(auto file_name: file_names) {
        // Deserialize json
        std::filesystem::path path{file_name};
        std::ifstream json_stream(path);
        nlohmann::ordered_json metatiles_json = nlohmann::json::parse(json_stream);

        // Setup file to write to
        path.replace_extension("bin");
        std::ofstream bin_file{path, std::ios::binary | std::ios::out};

        // Do some error checking
        if (metatiles_json["metatiles"].size() > metatiles_json["numMetatiles"]) {
            fprintf(stderr, "Warning: More metatiles than in numMetatiles for file %s.\n", path.c_str());
        }
        if (metatiles_json["metatiles"].size() > metatiles_json["numTiles"]) {
            fprintf(stderr, "Warning: More tiles than in numTiles for file %s.\n", path.c_str());
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
                fprintf(stderr, "Warning: Num tiles in metatile array differ to numTilesInMetatile for file %s.\n", path.c_str());
            }
            for (auto tile_data: metatile["tiles"]) {
                uint16_t temp_tile{0};
                for (const auto& [key, value]: masks) {
                    temp_tile |= static_cast<uint16_t>(tile_data[key].template get<uint16_t>() << firstBitOffset(value) & value);
                }
                tiles.push_back(temp_tile);
            }
        }

        // Write our bin file
        for (const uint16_t& tile: tiles) {
            bin_file.write(reinterpret_cast<const char *>(&tile), sizeof(tile));
        }
    }
}

// C++17 doesn't support templated lambdas, so we create a helper function here to call into with the fixed width type
template <typename T>
void parseMetatileAttributesFixedWidthInt(nlohmann::ordered_json& metatile_attribute_json, std::ofstream& bin_file, std::filesystem::path& path) {
    if (metatile_attribute_json["metatiles"].size() > metatile_attribute_json["numMetatiles"]) {
        fprintf(stderr, "Warning: More metatiles than in numMetatiles in file %s.\n", path.c_str());
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

    // Write our bin file
    for (const T& metatile: metatile_attributes_data) {
        bin_file.write(reinterpret_cast<const char *>(&metatile), sizeof(metatile));
    }
}

void convertMetatileAttributesJsonToBin(const std::vector<std::string>& file_names) {
    for(auto file_name: file_names) {
        // Deserialize json
        std::filesystem::path path{file_name};
        std::ifstream json_stream(path);
        nlohmann::ordered_json metatile_attribute_json = nlohmann::json::parse(json_stream);

        // Setup file to write to
        path.replace_extension("bin");
        std::ofstream bin_file{path, std::ios::binary | std::ios::out};

        // Since the size can vary, we template this function to handle various attribute sizes
        unsigned attribute_size_in_bits = {0};
        attribute_size_in_bits = metatile_attribute_json["attributeSizeInBits"].template get<unsigned>();
        if (attribute_size_in_bits == 8) {
            parseMetatileAttributesFixedWidthInt<uint8_t>(metatile_attribute_json, bin_file, path);
        } else if (attribute_size_in_bits == 16) {
            parseMetatileAttributesFixedWidthInt<uint16_t>(metatile_attribute_json, bin_file, path);
        } else if (attribute_size_in_bits == 32) {
            parseMetatileAttributesFixedWidthInt<uint32_t>(metatile_attribute_json, bin_file, path);
        }  else if (attribute_size_in_bits == 64) { // If someone actually uses this you're insane lol
            parseMetatileAttributesFixedWidthInt<uint64_t>(metatile_attribute_json, bin_file, path);
        } else {
            fprintf(stderr, "Warning: attributeSizeInBits either missing or not fixed width 8, 16, 32, 64\n");
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        FATAL_ERROR(Usage);
    }
    
    UsageMode mode = StrToMode(argv[1]);
    if (mode == UsageMode::Error) {
        FATAL_ERROR(Usage);
    }

    std::vector<std::string> file_names;

    for (int i = 2; i < argc; i++) {
        file_names.emplace_back(argv[i]);
    }

    switch(mode) {
        case UsageMode::Mapgrid:
            convertMapgridJsonToBin(file_names);
            break;
        case UsageMode::Metatiles:
            convertMetatilesJsonToBin(file_names);
            break;
        case UsageMode::MetatileAttributes:
            convertMetatileAttributesJsonToBin(file_names);
            break;
        case UsageMode::Error:
            FATAL_ERROR(Usage);
            break;
    }

    return 0;
}
