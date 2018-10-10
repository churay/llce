### Compilation/Linking Tools/Flags ###

CXX = clang++
CXX_FLAGS = -std=c++1y -Wall -fno-exceptions -g3 -O0 -DLLCE_DEBUG
# CXX_FLAGS = -std=c++1y -Wall -fno-exceptions -g0 -O3
CXX_INCLS = -I$(SRC_DIR) -Wl,-rpath=$(BIN_DIR)

CXX_LIB_FLAGS = `pkg-config --cflags sdl2 SDL2_ttf`
CXX_LIB_INCLS = `pkg-config --static --libs sdl2 SDL2_ttf`

### Project Files/Directories ###

PROJ_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BIN_DIR = $(PROJ_DIR)/bin
DAT_DIR = $(PROJ_DIR)/dat
OBJ_DIR = $(PROJ_DIR)/obj
OUT_DIR = $(PROJ_DIR)/out
ETC_DIR = $(PROJ_DIR)/etc
SRC_DIR = $(PROJ_DIR)/src

PROJ_EXE = $(BIN_DIR)/sdl
PROJ_MAIN = $(SRC_DIR)/sdl.cpp

EX_EXE = $(BIN_DIR)/tty
EX_MAIN = $(SRC_DIR)/tty.cpp

### Project Functions ###

# NOTE(JRC): Per-file locks aren't necessary when writing to files on Unix,
# so a special command is needed to inform other processes that a file is in use.
lock_command = touch $(1).lock && flock -x -w 1 $(1).lock -c $(2) && rm $(1).lock


### Project Targets ###

.PHONY : all

all : $(PROJ_EXE)

sdl : $(PROJ_EXE)
$(PROJ_EXE) : $(PROJ_MAIN) $(BIN_DIR)/platform.o $(BIN_DIR)/timer.o $(BIN_DIR)/memory.o $(BIN_DIR)/sdllib.so | $(BIN_DIR) $(OUT_DIR)
	$(call lock_command,$@,'$(CXX) $(CXX_FLAGS) -ldl -lGL $(CXX_LIB_FLAGS) $(CXX_INCLS) $(filter-out %.so,$^) -o $@ $(CXX_LIB_INCLS)')

tty : $(EX_EXE)
$(EX_EXE) : $(EX_MAIN) $(BIN_DIR)/platform.o $(BIN_DIR)/keyboard.o $(BIN_DIR)/timer.o $(BIN_DIR)/memory.o $(BIN_DIR)/ttylib.so | $(BIN_DIR) $(OUT_DIR)
	$(call lock_command,$@,'$(CXX) $(CXX_FLAGS) -ldl $(CXX_INCLS) $(filter-out %.so,$^) -o $@')

$(BIN_DIR)/%.so : $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	$(call lock_command,$@,'$(CXX) $(CXX_FLAGS) -fPIC -shared $< -o $@')

$(BIN_DIR)/%.o : $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	$(call lock_command,$@,'$(CXX) $(CXX_FLAGS) -c $< -o $@')

$(BIN_DIR) $(OBJ_DIR) $(OUT_DIR) :
	mkdir -p $@

clean :
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)
