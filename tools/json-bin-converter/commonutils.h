#pragma once

#ifndef COMMONUTILS_H
#define COMMONUTILS_H

#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <vector>

/// Converts a number of any type, into the hex representation in a string
template <typename T>
std::string intToHexStd(T num)
{
    std::stringstream stream;
    stream << "0x"
            << std::setfill ('0') << std::setw(sizeof(T)*2)
            << std::hex << num;
    return stream.str();
}

/// Converts a string that contains hex values to an int
template <typename T>
T hexToInt(std::string str)
{
    T retVal;
    std::stringstream stream;
    stream << std::hex << str;
    stream >> retVal;
    return retVal;
}

/// Uses some C++ shenanigans to get the num of bits represented in the variable
template <typename T>
constexpr unsigned sizeInBits(T var) noexcept
{
    return sizeof(typeid(T));
}

/// Keep bitshifting to the right until we get to the first set bit
template <typename T>
constexpr unsigned firstBitOffset(T var) noexcept
{
    unsigned max_size = sizeInBits(var);
    unsigned first_bit = 0;
    while (first_bit < max_size && ((var & 0x1) != 1)) {
        var = var >> 1;
        first_bit++;
    }
    return first_bit;
}

/// Takes in a file path that will be read from as a binary file, then outputs this data into a collection
///     of bytes to be read by something else
inline std::vector<std::byte> readBinFileIntoBuffer(const std::filesystem::path& file_path) {
    std::ifstream bin_file(file_path, std::ios_base::binary);

    bin_file.seekg(0, std::ios_base::end);
    auto length = bin_file.tellg();
    bin_file.seekg(0, std::ios_base::beg);

    std::vector<std::byte> buffer(length);
    bin_file.read(reinterpret_cast<char*>(buffer.data()), length);

    // Wooo copy elision :sparkles:
    return buffer;
}

/// This checks the filepath to see if the keyword "secondary" is found. Users can reorganze their directories,
///     so we don't want to assume the default of data/tilesets/secondary/* but we will assume secondary is found
///     _somewhere_ in their path for this metatile / metatile attr path.
inline bool filepathContainsSecondary(const std::filesystem::path& file_path) {
    for (auto it = file_path.begin(); it != file_path.end(); ++it) {
        if (*it == "secondary") {
            return true;
        }
    }
    return false;
}

/// Warn the user if we're expecting a bin file extension, but a different extension is provided
inline bool verifyBinFileExtension(std::filesystem::path file_path) {
   if (file_path.extension() != ".bin") {
      fprintf(stderr, "Warning: '.bin' file extension missing for file %s. If this is intentional you can ignore this message.\n", file_path.c_str());
      return false;
   }
   return true;
}

/// Warn the user if we're expecting a json file extension, but a different extension is provided
inline bool verifyJsonFileExtension(std::filesystem::path file_path) {
   if (file_path.extension() != ".json") {
      fprintf(stderr, "Warning: '.json' file extension missing for file %s. If this is intentional you can ignore this message.\n", file_path.c_str());
      return false;
   }
   return true;
}

#endif // COMMONUTILS_H
