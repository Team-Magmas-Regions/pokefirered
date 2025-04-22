/// bin2json_main.cpp
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
#include "bin2json.h"

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

    // Process
    switch (mode) {
        case UsageMode::Mapgrid:
            for (auto file_name: file_names) {        
                bin2json::convertMapgridBinToJson(file_name, version);
            }
            break;
        case UsageMode::Metatiles:
            for (auto file_name: file_names) {        
                bin2json::convertMetatilesBinToJson(file_name, version);
            }
            break;
        case UsageMode::MetatileAttributes:
            for (auto file_name: file_names) {        
                bin2json::convertMetatileAttributesBinToJson(file_name, version);
            }
            break;
        case UsageMode::Error:
            FATAL_ERROR(Usage);
            break;
    }

    return 0;
}
