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

cmake_minimum_required(VERSION 3.16)
project(safetynet LANGUAGES C)

set(CMAKE_SKIP_INSTALL_RULES ON)

# Set output directory
set(BIN_DIR ${CMAKE_BINARY_DIR}/build)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Enable position-independent code (like -fPIC)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Debug flag control
option(DEBUG "Enable debug mode" ON)

# Compiler flags
if (DEBUG)
    set(DEBUG_FLAGS "-g -O0")
else()
    set(DEBUG_FLAGS "-g -O1")
endif()

if(DEFINED ENV{VER})
    set(VER_FLAG_S "$ENV{VER}")
else()
    set(VER_FLAG_S "dev")
endif()

# Common flags
set(GCC_FLAGS
    "${DEBUG_FLAGS} -DSN_BUILD_VER=\"${VER_FLAG_S}\" -Wall -Werror -fno-strict-aliasing -fvisibility=hidden -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function"
)

# Set C and C++ flags
set(CMAKE_C_FLAGS "${GCC_FLAGS} -std=c99 -D__SN_WIP_CALLS__=")
#set(CMAKE_CXX_FLAGS "${GCC_FLAGS} -std=gnu++17")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Collect source files
file(GLOB C_SOURCES "${SRC_DIR}/*.c")
file(GLOB CXX_SOURCES "${SRC_DIR}/*.cpp")

# Shared library
add_library(safetynet_shared SHARED ${C_SOURCES})
set_target_properties(safetynet_shared PROPERTIES
    OUTPUT_NAME "safetynet"
    LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}
)

# Static library
add_library(safetynet_static STATIC ${C_SOURCES})
set_target_properties(safetynet_static PROPERTIES
    OUTPUT_NAME "safetynet"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
)


# Optional: add test rule
add_custom_target(mem_test
    COMMAND ${CMAKE_SOURCE_DIR}/test.sh
    DEPENDS safetynet_shared
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
