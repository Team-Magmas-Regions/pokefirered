CXX ?= g++

CXXFLAGS := -Wall -std=c++17 -O2

INCLUDES := -I../libs

BIN2JSON_SRCS := bin2json.cpp

JSON2BIN_SRCS := json2bin.cpp

HEADERS := jsonbinconverter.h commonutils.h ../libs/nlohmann/json.hpp

BIN2JSON_HEADERS := datainfos.h custominfos.h

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: bin2json$(EXE) json2bin$(EXE) 
	@:

bin2json$(EXE): $(BIN2JSON_SRCS) $(HEADERS) $(BIN2JSON_HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(BIN2JSON_SRCS) -o $@ $(LDFLAGS)

json2bin$(EXE): $(JSON2BIN_SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(JSON2BIN_SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) bin2json bin2json.exe
	$(RM) json2bin json2bin.exe
