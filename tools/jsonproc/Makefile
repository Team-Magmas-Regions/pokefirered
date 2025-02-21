CXX ?= g++

CXXFLAGS := -Wall -std=c++17 -O2

INCLUDES := -I../libs

SRCS := jsonproc.cpp

HEADERS := jsonproc.h ../libs/inja.hpp ../libs/nlohmann/json.hpp

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: jsonproc$(EXE)
	@:

jsonproc$(EXE): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) jsonproc jsonproc.exe
