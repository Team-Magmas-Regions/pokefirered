/// bin2json.h
/// Contains function definitions for conversion from bin files to json files
/// for mapgrid, metatiles, and metatile attributes
#pragma once

#ifndef BIN2JSON_H
#define BIN2JSON_H

#include "jsonbinconverter.h"
#include "datainfos.h"
#include "custominfos.h"

#include <vector>
#include <nlohmann/json.hpp>


namespace bin2json {

/// Given a file name and version, loads file a binary file, and attempts to output a json file
///   as a sibling to the original bin file. Will throw a warning if the file extension is not
///   bin, but will proceed as usual
void convertMapgridBinToJson(const std::string &file_name, Version version);

/// Takes in a collection of bytes that is assumed to follow the mapgrid format as
///   detailed in the MapGridInfo struct, which can then be serialized to json
nlohmann::ordered_json parseMapgridBytes(const MapGridInfo& info, const std::vector<std::byte>& buffer);

/// Given a file name and version, loads file a binary file, and attempts to output a json file
///   as a sibling to the original bin file. Will throw a warning if the file extension is not
///   bin, but will proceed as usual
void convertMetatilesBinToJson(const std::string &file_name, Version version);

/// Takes in a collection of bytes that is assumed to follow the metatiles format as
///   detailed in the MetatilesInfo struct, which can then be serialized to json
nlohmann::ordered_json parseMetatilesBytes(const MetatilesInfo& info, bool is_secondary, const std::vector<std::byte>& buffer);

/// Given a file name and version, loads file a binary file, and attempts to output a json file
///   as a sibling to the original bin file. Will throw a warning if the file extension is not
///   bin, but will proceed as usual
void convertMetatileAttributesBinToJson(const std::string &file_name, Version version);

/// Takes in a collection of bytes that is assumed to follow the metatile attributes format
///   as detailed in the MetatileAttributeInfo struct, which can then be serialized to json
template<class T>
nlohmann::ordered_json parseMetatileAttributesBytes(const MetatileAttributesInfo<T>& info, bool is_secondary, const std::vector<std::byte>& buffer);

}

#endif // BIN2JSON_H