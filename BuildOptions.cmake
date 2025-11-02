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
message(NOTICE "system triple-prefixed is ${PLATFORM_TRIPLE} on compiler ${CMAKE_C_COMPILER}")

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
    find_program(PLAT_NM
            NAMES
            ${PLATFORM_TRIPLE}-nm
            nm
            DOC "Platform-appropriate objcopy tool"
    )
else()
    find_program(PLAT_OBJCOPY
            NAMES
            objcopy
            DOC "Default objcopy tool"
    )
    find_program(PLAT_NM
            NAMES
            nm
            DOC "Default objcopy tool"
    )
endif()

if(NOT PLAT_OBJCOPY)
    message(FATAL_ERROR "Failed to locate objcopy for platform '${PLATFORM_TRIPLE}'. Please install binutils.")
else()
    message(STATUS "Using objcopy: ${PLAT_OBJCOPY}")
endif()

if(NOT PLAT_NM)
    message(FATAL_ERROR "Failed to locate nm for platform '${PLATFORM_TRIPLE}'. Please install binutils.")
else()
    message(STATUS "Using nm: ${PLAT_NM}")
endif()

if(NOT WIN32)
    option(SN_CONFIG_STATIC_ONLY "If to produce a static only library" OFF)
    option(SN_CONFIG_NO_TESTING_SUITE "Disable the test suite if set" OFF)
else ()
    option(SN_CONFIG_STATIC_ONLY "If to produce a static only library" ON)
    option(SN_CONFIG_NO_TESTING_SUITE "Disable the test suite if set" ON)
endif ()

option(SN_NO_STD_BOOL "Does not uses stdbool.h" OFF)

option(SN_CONFIG_SANITIZE_MEMORY_ON_FREE "pressure wash the block of memory with zeros on free" ON)

# Debug flag control
option(SN_CONFIG_DEBUG "Enable debug mode" ON)

option(SN_CONFIG_ENABLE_MUTEX "Use thread safety mechanisms" ON)
option(SN_CONFIG_ENABLE_PRIMITIVE_STACK_TRACE "On library crash it will show you a primitive stack trace" ON)
option(SN_CONFIG_ENABLE_DUMP_LIST_CRASH "On library crash it will All the linked list nodes which can get big" ON)


string(TIMESTAMP SN_CONFIG_GENERATION_DATE "%m-%d-%Y(%H:%M:%S)")
string(TIMESTAMP SN_CONFIG_GENERATION_YEAR "%Y")


function(set_target_output target_name out_path)
    set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${out_path}"
            LIBRARY_OUTPUT_DIRECTORY "${out_path}"
            ARCHIVE_OUTPUT_DIRECTORY "${out_path}"
    )
endfunction()

execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE GIT_BRANCH_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)