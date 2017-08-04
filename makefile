### Compilation/Linking Tools/Flags ###

CXX = clang++
CXX_FLAGS = -std=c++1y -Wall -g -O0
CXX_INCLS = -I$(SRC_DIR)

CXX_LIB_FLAGS = `pkg-config --cflags sdl2`
CXX_LIB_INCLS = `pkg-config --static --libs sdl2`

### Project Files/Directories ###

PROJ_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BIN_DIR = $(PROJ_DIR)/bin
OBJ_DIR = $(PROJ_DIR)/obj
SRC_DIR = $(PROJ_DIR)/src

PROJ_EXE = $(BIN_DIR)/llce
PROJ_MAIN = $(SRC_DIR)/llce.cpp

EX_EXE = $(BIN_DIR)/dyload
EX_MAIN = $(SRC_DIR)/dyload.cpp

.PHONY : all

all : $(PROJ_EXE)

llce : $(PROJ_EXE)
$(PROJ_EXE) : $(PROJ_MAIN) $(SRC_DIR)/texture.cpp $(SRC_DIR)/timer.cpp | $(BIN_DIR)
	$(CXX) $(CXX_FLAGS) $(CXX_LIB_FLAGS) $(CXX_INCLS) $^ -o $@ $(CXX_LIB_INCLS)

dyload : $(EX_EXE)
$(EX_EXE) : $(EX_MAIN) $(SRC_DIR)/timer.cpp $(BIN_DIR)/dylib.so | $(BIN_DIR)
	$(CXX) $(CXX_FLAGS) $(CXX_INCLS) $^ -o $@

$(BIN_DIR)/%.so : $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	$(CXX) $(CXX_FLAGS) -fPIC -shared $< -o $@

$(BIN_DIR) $(OBJ_DIR) :
	mkdir -p $@

clean :
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)
