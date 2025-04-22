/// json2bin.h
/// Contains function definitions for conversion from json files to bin files
///   for mapgrid, metatiles, and metatile attributes
#pragma once

#ifndef JSON2BIN_H
#define JSON2BIN_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <filesystem>

namespace json2bin {

/// Given a file name, loads file as json object, and attempts to output
///   a bin file as a sibling to the original json file. Will throw a
///   warning if the file extension is not json, but will proceed as usual
void convertMapgridJsonToBin(const std::string &file_name);

/// Takes in an ordered json that is assumed to follow the mapgrid format as
///   detailed in the MapGridInfo struct, which can then be serialized to bin
std::vector<uint16_t> parseMapgridJson(const nlohmann::ordered_json &mapgrid_json);

/// Given a file name, loads file as json object, and attempts to output
///   a bin file as a sibling to the original json file. Will throw a
///   warning if the file extension is not json, but will proceed as usual
void convertMetatilesJsonToBin(const std::string &file_name);

/// Takes in an ordered json that is assumed to follow the metatiles format as
///   detailed in the MetatilesInfo struct, which can then be serialized to bin
std::vector<uint16_t> parseMetatilesJson(nlohmann::ordered_json metatiles_json,
                                         std::optional<std::filesystem::path> path);

/// Given a file name, loads file as json object, and attempts to output
///   a bin file as a sibling to the original json file. Will throw a
///   warning if the file extension is not json, but will proceed as usual
void convertMetatileAttributesJsonToBin(const std::string &file_name);

/// Takes in an ordered json that is assumed to follow the metatile attribute format
///   as detailed in the MetatileAttributeInfo struct, which can then be serialized to bin
template <typename T>
std::vector<T> parseMetatileAttributesJson(nlohmann::ordered_json& metatile_attribute_json,
                                           std::optional<std::filesystem::path> path);

}

#endif // JSON2BIN_H