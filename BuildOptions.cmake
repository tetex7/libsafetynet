#
# Copyright (C) 2024  Tetex7
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

execute_process(
        COMMAND ${CMAKE_C_COMPILER} -dumpmachine
        OUTPUT_VARIABLE PLATFORM_TRIPLE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)


# --- Automatically pick objcopy binary ---
# 1. Try triple-prefixed version (e.g. x86_64-linux-gnu-objcopy)
# 2. Fall back to plain objcopy if triple is empty or prefixed tool doesn't exist
if(PLATFORM_TRIPLE)
    find_program(PLAT_OBJCOPY
            NAMES
            ${PLATFORM_TRIPLE}-objcopy
            objcopy
            DOC "Platform-appropriate objcopy tool"
    )
else()
    find_program(PLAT_OBJCOPY
            NAMES
            objcopy
            DOC "Default objcopy tool"
    )
endif()

if(NOT PLAT_OBJCOPY)
    message(FATAL_ERROR "Failed to locate objcopy for platform '${PLATFORM_TRIPLE}'. Please install binutils.")
else()
    message(STATUS "Using objcopy: ${PLAT_OBJCOPY}")
endif()

if(NOT WIN32)
    option(SN_CONFIG_STATIC_ONLY "If to produce a static only library" OFF)
else ()
    option(SN_CONFIG_STATIC_ONLY "If to produce a static only library" ON)
endif ()

option(SN_NO_STD_BOOL "uses stdbool.h" OFF)

option(SN_CONFIG_SANITIZE_MEMORY_ON_FREE "pressure wash the block of memory with zeros on free" ON)

# Debug flag control
option(SN_CONFIG_DEBUG "Enable debug mode" OFF)

# Version variables are changed at compile time by Package script
set(SN_CONFIG_STR_VERSION "0.0.0" CACHE STRING "Set the version being compiled")
set(SN_CONFIG_VERSION_MAJOR 0 CACHE STRING "Set the major version being compiled")
set(SN_CONFIG_VERSION_MINOR 0 CACHE STRING "Set the minor version being compiled")
set(SN_CONFIG_VERSION_PATCH 0 CACHE STRING "Set the patch version being compiled")

option(SN_CONFIG_ENABLE_MUTEX "Use thread safety mechanisms" ON)


string(TIMESTAMP SN_CONFIG_GENERATION_DATE "%m-%d-%Y(%H:%M:%S)")
string(TIMESTAMP SN_CONFIG_GENERATION_YEAR "%Y")

execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE GIT_BRANCH_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)