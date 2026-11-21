/// jsonbinconverter_testsuite.cpp
/// A set of UwUnit tests to guarantee successful execution and feature completeness in conversion to and
/// from bin and json for metatiles, metatile attributes, and mapgrid

#include "jsonbinconverter.h"
#include "commonutils.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "json2bin.h"
#include "bin2json.h"
#include "testdata.h"


int main(int argc, char *argv[]) {
    unsigned failureCount = 0;

    auto checkFailuresAndReset = [&failureCount](const std::string testArea, unsigned testTotal) {
        auto testsTotalCount = [&testTotal, &failureCount]() -> std::string {
            return (std::stringstream{} << "[" << testTotal << "/" << testTotal << "]").str();
        };
        std::cout << "Testing " << testArea << ": " << testsTotalCount();
        if (failureCount == 0) {
            std::cout << " Success UwU";
        } else {
            std::cout << " - OwO no! " << failureCount << " tests failed ;_;";
        }
        std::cout << std::endl;
        failureCount = 0;
    };

    // Check individual metatiles, metatile atributes, and map grid tiles
    for (uint32_t bitField : testdata::bitFieldPatterns32) {
        // TODO: Parse single metatile attr into json

        // TODO: Parse returned json into metatile attr bin

        // TODO: Compare roundtrip success
    }
    checkFailuresAndReset("MetatileAttr32", testdata::bitFieldPatterns32.size());

    for (auto bitField : testdata::bitFieldPatterns16) {
        // TODO: Parse single metatile attr into json

        // TODO: Parse returned json into metatile attr bin

        // TODO: Compare roundtrip success
    }
    checkFailuresAndReset("MetatileAttr16", testdata::bitFieldPatterns16.size());

    for (auto bitField : testdata::bitFieldPatterns16) {
        // TODO: Parse single metatile into json

        // TODO: Parse returned json into metatile bin

        // TODO: Compare roundtrip success
    }
    checkFailuresAndReset("Metatiles", testdata::bitFieldPatterns16.size());

    for (auto bitField : testdata::bitFieldPatterns16) {
        // TODO: Parse single mapgrid tile into json

        // TODO: Parse returned json into mapgrid bin

        // TODO: Compare roundtrip success
    }
    checkFailuresAndReset("LayoutMapgrids", testdata::bitFieldPatterns16.size());


    // Check full file translations
    {
        // TODO: Parse full metatile attr file into json

        // TODO: Parse returned json into full metatile attr bin

        // TODO: Compare roundtrip success
    }
    checkFailuresAndReset("MetatileAttr32FullFile", 2);

    {
        // TODO: Parse full metatile attr file into json

        // TODO: Parse returned json into full metatile attr bin

        // TODO: Compare roundtrip success
    }
    checkFailuresAndReset("MetatileAttr16FullFile", 2);

    {
        // TODO: Parse full metatile file into json

        // TODO: Parse returned json into full metatile bin

        // TODO: Compare roundtrip success
    }
    checkFailuresAndReset("MetatilesFullFile", 2);

    {
        // TODO: Parse full mapgrid file into json

        // TODO: Parse returned full json into mapgrid bin

        // TODO: Compare roundtrip success
    }
    checkFailuresAndReset("LayoutMapgridsFullFile", 2);

    return 0;
}
