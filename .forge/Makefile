.ONESHELL:
.SILENT:

# Project name
PROJECT := forge

# Final executable
EXE := $(PROJECT).bin

# Utilities
MKDIR := mkdir
RM := rm
ECHO := echo

# Directories
ROOT_DIR := 

# Internal forge directories
FORGE_DIR := .forge
FORGE_BIN_DIR := $(FORGE_DIR)/bin
FORGE_BIN_MODULES_DIR := $(FORGE_BIN_DIR)/modules
FORGE_BIN_DEBUG_DIR := $(FORGE_BIN_DIR)/debug
FORGE_BIN_DEBUG_OBJ_DIR := $(FORGE_BIN_DEBUG_DIR)/obj
FORGE_BIN_RELEASE_DIR := $(FORGE_BIN_DIR)/release
FORGE_BIN_RELEASE_OBJ_DIR := $(FORGE_BIN_RELEASE_DIR)/obj

SRC_DIR := src
MODULES_DIR := modules

# Compiler
CC := gcc

# Source file type
SRC_TYPE := c

# Source files
SRCS := $(shell find $(SRC_DIR) -type f -name "*.$(SRC_TYPE)")
DEBUG_OBJS := $(addprefix $(FORGE_BIN_DEBUG_OBJ_DIR)/,$(SRCS:src/%.$(SRC_TYPE)=%.o))
RELEASE_OBJS := $(addprefix $(FORGE_BIN_RELEASE_OBJ_DIR)/,$(SRCS:src/%.$(SRC_TYPE)=%.o))

# Compilation flags
LINKER_FLAGS :=
INCLUDE_FLAGS := -I$(MODULES_DIR)
RELEASE_FLAGS := -Wextra -Wall -Werror -Wno-vla -std=c99 -O2 $(LINKER_FLAGS) $(INCLUDE_FLAGS) -DNDEBUG
DEBUG_FLAGS := -Wextra -Wall -Wno-vla -std=c99 -g3 $(LINKER_FLAGS) $(INCLUDE_FLAGS)

# Arguments to be passed when running "make run Args="<arguments>"
ARGS :=

.PHONY: all run debug release clean directories clean-release clean-debug modules

all: debug

run: debug
	./$(FORGE_BIN_DEBUG_DIR)/$(EXE) $(ARGS)

debug: directories $(DEBUG_OBJS)
	$(CC) $(DEBUG_FLAGS) $(DEBUG_OBJS) $(FORGE_BIN_MODULES_DIR)/*.o -o $(FORGE_BIN_DEBUG_DIR)/$(EXE)

release: directories clean-release $(RELEASE_OBJS)
	$(CC) $(RELEASE_FLAGS) $(RELEASE_OBJS) $(FORGE_BIN_MODULES_DIR)/*.o -o $(FORGE_BIN_RELEASE_DIR)/$(EXE)

clean: clean-release clean-debug

directories:
	$(MKDIR) -p $(FORGE_DIR)
	$(MKDIR) -p $(FORGE_BIN_DIR)
	$(MKDIR) -p $(FORGE_BIN_MODULES_DIR)
	$(MKDIR) -p $(FORGE_BIN_DEBUG_DIR)
	$(MKDIR) -p $(FORGE_BIN_DEBUG_OBJ_DIR)
	$(MKDIR) -p $(FORGE_BIN_RELEASE_DIR)
	$(MKDIR) -p $(FORGE_BIN_RELEASE_OBJ_DIR)
	$(MKDIR) -p $(SRC_DIR)
	$(MKDIR) -p $(MODULES_DIR)

clean-release:
	$(RM) -f $(RELEASE_OBJS)
	$(RM) -f $(FORGE_BIN_RELEASE_DIR)/$(EXE)
	$(RM) -rf $(FORGE_BIN_RELEASE_OBJ_DIR)/*

clean-debug:
	$(RM) -f $(DEBUG_OBJS)
	$(RM) -f $(FORGE_BIN_DEBUG_DIR)/$(EXE)
	$(RM) -rf $(FORGE_BIN_DEBUG_OBJ_DIR)/*

$(DEBUG_OBJS): $(FORGE_BIN_DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_TYPE)
	$(MKDIR) -p $(dir $@)
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

$(RELEASE_OBJS): $(FORGE_BIN_RELEASE_OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_TYPE)
	$(MKDIR) -p $(dir $@)
	$(CC) $(RELEASE_FLAGS) -c $< -o $@

# Modules
include $(FORGE_DIR)/modules.mk

modules: c-vector optparse99 tomlc99
