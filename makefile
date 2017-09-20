### Compilation/Linking Tools/Flags ###

CXX = clang++
CXX_FLAGS = -std=c++1y -Wall -fno-exceptions -g -O0 -DDEBUG
CXX_INCLS = -I$(SRC_DIR) -Wl,-rpath=$(BIN_DIR)

CXX_LIB_FLAGS = `pkg-config --cflags sdl2`
CXX_LIB_INCLS = `pkg-config --static --libs sdl2`

### Project Files/Directories ###

PROJ_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BIN_DIR = $(PROJ_DIR)/bin
OBJ_DIR = $(PROJ_DIR)/obj
OUT_DIR = $(PROJ_DIR)/out
SRC_DIR = $(PROJ_DIR)/src

PROJ_EXE = $(BIN_DIR)/llce
PROJ_MAIN = $(SRC_DIR)/llce.cpp

EX_EXE = $(BIN_DIR)/dyload
EX_MAIN = $(SRC_DIR)/dyload.cpp

### Project Functions ###

# NOTE(JRC): Per-file locks aren't necessary when writing to files on Unix,
# so a special command is needed to inform other processes that a file is in use.
lock_command = touch $(1).lock && flock -x -w 1 $(1).lock -c $(2) && rm $(1).lock

### Project Targets ###

.PHONY : all

all : $(PROJ_EXE)

llce : $(PROJ_EXE)
$(PROJ_EXE) : $(PROJ_MAIN) $(SRC_DIR)/texture.cpp $(SRC_DIR)/timer.cpp | $(BIN_DIR) $(OUT_DIR)
	$(call lock_command,$@,'$(CXX) $(CXX_FLAGS) $(CXX_LIB_FLAGS) $(CXX_INCLS) $^ -o $@ $(CXX_LIB_INCLS)')

dyload : $(EX_EXE)
$(EX_EXE) : $(EX_MAIN) $(BIN_DIR)/platform.o $(BIN_DIR)/keyboard.o $(BIN_DIR)/timer.o $(BIN_DIR)/dylib.so | $(BIN_DIR) $(OUT_DIR)
	$(call lock_command,$@,'$(CXX) $(CXX_FLAGS) -ldl $(CXX_INCLS) $(filter-out %.so,$^) -o $@')

$(BIN_DIR)/%.so : $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	$(call lock_command,$@,'$(CXX) $(CXX_FLAGS) -fPIC -shared $< -o $@')

$(BIN_DIR)/%.o : $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	$(call lock_command,$@,'$(CXX) $(CXX_FLAGS) -c $< -o $@')

$(BIN_DIR) $(OBJ_DIR) $(OUT_DIR) :
	mkdir -p $@

clean :
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)
