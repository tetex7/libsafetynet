#
# Copyright (C) 2024  tete
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

DEBUG ?= 1

ifeq ($(DEBUG),1)
  DEBUG_FLAGS += -g -O0
else
  DEBUG_FLAGS += -O1 -g
endif

# Compiler and flags
CC = ${PREFIX}gcc
CXX = ${PREFIX}g++
GCCFLAG = ${DEBUG_FLAGS} -Wall -Werror -fno-strict-aliasing -fvisibility=hidden -fPIC -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function # -fPIC for Position Independent Code
CFLAGS = ${GCCFLAG} -std=gnu99 -D__SN_WIP_CALLS__=
CPPFLAGS = ${GCCFLAG} -std=gnu++17
LDFLAGS = ${DEBUG_FLAGS} -Wall -fno-strict-aliasing -fvisibility=hidden -shared  # Link as a shared object
LDLIBS =

# Directories
SRC_DIR = src
BIN_DIR = build
OBJ_DIR = ${BIN_DIR}
INCLUDE_DIRS = -I./include -I./src

# Source files and object files
C_SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
CPP_SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
ASM_SRC_FILES = $(wildcard $(SRC_DIR)/*.S)

# Object files for C and C++ sources
C_OBJ_FILES = $(C_SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.c.o)
CPP_OBJ_FILES = $(CPP_SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.cpp.o)
S_OBJ_FILES = $(CPP_SRC_FILES:$(SRC_DIR)/%.S=$(OBJ_DIR)/%.S.o)
# All object files
OBJ_FILES = $(C_OBJ_FILES) $(CPP_OBJ_FILES)
TARGET = $(BIN_DIR)/libsafetynet.so

# Default rule: build the target
all: $(TARGET)

# Rule to link object files into the shared library
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o $(TARGET) $(LDLIBS)

# Rule to compile source files into object files
$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $(OBJ_DIR)/$*.c.o

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCLUDE_DIRS) -c $< -o $(OBJ_DIR)/$*.cpp.o

$(OBJ_DIR)/%.S.o: $(SRC_DIR)/%.S
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $(OBJ_DIR)/$*.S.o

# Rule for cleaning up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

mem_test: all
	@./test.sh

# Rule to remove only object files
clean-obj:
	rm -rf $(OBJ_DIR)/*.c.o

# Rule to remove only the shared library
clean-bin:
	rm -rf $(BIN_DIR)/$(notdir $(TARGET))

# Rule to build and run tests (if you add tests in the future)
test: mem_test

# Phony targets (not actual files)
.PHONY: all clean clean-obj clean-bin test