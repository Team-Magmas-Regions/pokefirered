CXX ?= g++

CXXFLAGS := -Wall -std=c++17 -O2

INCLUDES := -I../libs

BIN2JSON_SRCS := bin2json.cpp bin2json_main.cpp

JSON2BIN_SRCS := json2bin.cpp json2bin_main.cpp

COMMON_HEADERS := jsonbinconverter.h commonutils.h ../libs/nlohmann/json.hpp

BIN2JSON_HEADERS := bin2json.h datainfos.h custominfos.h

JSON2BIN_HEADERS := json2bin.h

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: bin2json$(EXE) json2bin$(EXE) 
	@:

bin2json$(EXE): $(BIN2JSON_SRCS) $(COMMON_HEADERS) $(BIN2JSON_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(BIN2JSON_SRCS) -o $@ $(LDFLAGS)

json2bin$(EXE): $(JSON2BIN_SRCS) $(COMMON_HEADERS) $(JSON2BIN_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(JSON2BIN_SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) bin2json bin2json.exe
	$(RM) json2bin json2bin.exe
