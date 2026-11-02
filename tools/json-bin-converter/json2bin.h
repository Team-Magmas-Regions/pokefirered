/// json2bin.h
/// Contains function definitions for conversion from json files to bin files
///   for mapgrid, metatiles, and metatile attributes
#pragma once

#ifndef JSON2BIN_H
#define JSON2BIN_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>
#include <string>
#include <filesystem>

namespace json2bin {

/// Given a file name, loads file as json object, and attempts to output
///   a bin file as a sibling to the original json file. Will throw a
///   warning if the file extension is not json, but will proceed as usual
/// Note: Touches the filesystem
/// Calls into parseMapgridJson
void convertMapgridJsonToBin(const std::string &file_name);

/// Takes in an ordered json that is assumed to follow the mapgrid format as
///   detailed in the MapGridInfo struct, which can then be serialized to bin
/// Calls into deserializeSingleMapgridTile
std::vector<uint16_t> parseMapgridJson(const nlohmann::ordered_json &mapgrid_json);

/// Parse an individual tile
/// Called from within parseMapgridJson
uint16_t deserializeSingleMapgridTile(const std::map<std::string, uint16_t>& masks,
                                    const nlohmann::ordered_json& tile_data);


/// Given a file name, loads file as json object, and attempts to output
///   a bin file as a sibling to the original json file. Will throw a
///   warning if the file extension is not json, but will proceed as usual
/// Note: Touches the filesystem
/// Calls into parseMetatilesJson
void convertMetatilesJsonToBin(const std::string &file_name);

/// Takes in an ordered json that is assumed to follow the metatiles format as
///   detailed in the MetatilesInfo struct, which can then be serialized to bin
/// path is only used for errors when called from convertMetatilesJsonToBin
/// Calls into deserializeSingleMetatiles
std::vector<uint16_t> parseMetatilesJson(nlohmann::ordered_json metatiles_json,
                                         std::optional<std::filesystem::path> path);

/// Parse an individual metatile
/// Called from within parseMetatilesJson
std::vector<uint16_t> deserializeSingleMetatile(std::map<std::string, uint16_t> masks,
                                                const nlohmann::ordered_json& metatile);


/// Given a file name, loads file as json object, and attempts to output
///   a bin file as a sibling to the original json file. Will throw a
///   warning if the file extension is not json, but will proceed as usual
/// Note: Touches the filesystem
void convertMetatileAttributesJsonToBin(const std::string &file_name);

/// Takes in an ordered json that is assumed to follow the metatile attribute format
///   as detailed in the MetatileAttributeInfo struct, which can then be serialized to bin
/// path is only used for errors when called from convertMetatilesJsonToBin
/// Calls into deserializeSingleMetatileAttribute
template <typename T>
std::vector<T> parseMetatileAttributesJson(const nlohmann::ordered_json& metatile_attribute_json,
                                           std::optional<std::filesystem::path> path);

/// Parse an individual metatile
/// Called from within parseMetatileAttributesJson
template <typename T>
T deserializeSingleMetatileAttribute(std::map<std::string, T> masks, const nlohmann::ordered_json& metatile);

}

#endif // JSON2BIN_H