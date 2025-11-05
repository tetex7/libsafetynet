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
if(CMAKE_SYSTEM_NAME MATCHES ".*BSD")
    set(IS_BSD ON)
else()
    set(IS_BSD OFF)
endif()

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

option(SN_CONFIG_VERBOSE_LOGGING_FACILITIES_FEATURE "If enabled Adds software facilities for verbose logging of memory actions" ON)

option(SN_CONFIG_SHARED_LIBRARY_POST_PROCESSING "Mangles private symbols in the shared library" OFF)

option(SN_NO_STD_BOOL "Does not uses stdbool.h" OFF)

option(SN_CONFIG_SANITIZE_MEMORY_ON_FREE "pressure wash the block of memory with zeros on free" ON)

# Debug flag control
option(SN_CONFIG_DEBUG "Enable debug mode" ON)

option(SN_CONFIG_ENABLE_MUTEX "Use thread safety mechanisms" ON)
option(SN_CONFIG_ENABLE_STACK_TRACE "On library crash it will show you a primitive stack trace" OFF)
option(SN_CONFIG_ENABLE_DUMP_LIST_CRASH "On library crash it will All the linked list nodes which can get big" ON)


string(TIMESTAMP SN_CONFIG_GENERATION_DATE "%m-%d-%Y(%H:%M:%S)")
string(TIMESTAMP SN_CONFIG_GENERATION_YEAR "%Y")


option(SN_USE_CPACK_PACKAGING "If Cpack packaging should be enabled" ON)

if(SN_CONFIG_ENABLE_STACK_TRACE AND SN_CONFIG_STATIC_ONLY)
    message(FATAL_ERROR "Producing a static build with stack trace enabled Is an invalid state")
endif ()

function(set_target_output target_name out_path)
    set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${out_path}"
            LIBRARY_OUTPUT_DIRECTORY "${out_path}"
            ARCHIVE_OUTPUT_DIRECTORY "${out_path}"
    )
endfunction()

execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_COMMIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE GIT_BRANCH_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)


if(SN_CONFIG_STATIC_ONLY AND SN_CONFIG_SHARED_LIBRARY_POST_PROCESSING)
    message(WARNING "Can't have both static-only and shared-library post-processing enabled. Ignored SN_CONFIG_SHARED_LIBRARY_POST_PROCESSING")
endif()

#if(SN_CONFIG_ENABLE_STACK_TRACE)
#    message(WARNING "Stack trace may be unstable.")
#endif()

# --- Create a custom "config_info" target ---
set(CONFIG_SUMMARY "${CMAKE_BINARY_DIR}/CMakeFiles/config_summary.txt")

file(WRITE ${CONFIG_SUMMARY} "========= libsafetynet Build Configuration =========\n")
file(APPEND ${CONFIG_SUMMARY} "Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform: ${CMAKE_SYSTEM_NAME}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform Triple: ${PLATFORM_TRIPLE}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform C Compiler: ${CMAKE_C_COMPILER}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform Objcopy Binary: ${PLAT_OBJCOPY}\n")
file(APPEND ${CONFIG_SUMMARY} "Platform NM Binary: ${PLAT_NM}\n")
file(APPEND ${CONFIG_SUMMARY} "Git: ${GIT_BRANCH_NAME} (${GIT_COMMIT_HASH})\n\n")
file(APPEND ${CONFIG_SUMMARY} "Feature Flags:\n")
foreach(flag
        SN_CONFIG_STATIC_ONLY
        SN_CONFIG_DEBUG
        SN_CONFIG_ENABLE_MUTEX
        SN_CONFIG_VERBOSE_LOGGING_FACILITIES_FEATURE
        SN_CONFIG_SANITIZE_MEMORY_ON_FREE
        SN_CONFIG_ENABLE_PRIMITIVE_STACK_TRACE
        SN_CONFIG_ENABLE_DUMP_LIST_CRASH
)
    file(APPEND ${CONFIG_SUMMARY} "  ${flag} = ${${flag}}\n")
endforeach()
file(APPEND ${CONFIG_SUMMARY} "====================================================\n")

# target to print it using cmake -E cat (no shell parsing)
add_custom_target(config_info
        COMMAND ${CMAKE_COMMAND} -E cat ${CONFIG_SUMMARY}
        COMMENT "Display the current libsafetynet configuration"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)