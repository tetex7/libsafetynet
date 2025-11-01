#
# Copyright (C) 2025  Tetex7
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


if(NOT DEFINED SN_CONFIG_VERSION)
    set(SN_CONFIG_VERSION 0.0.0)
endif()

project(safetynet VERSION ${SN_CONFIG_VERSION} LANGUAGES C)

include(BuildOptions.cmake)

set(CMAKE_SKIP_INSTALL_RULES ON)


# Set output directory
set(BIN_DIR ${CMAKE_BINARY_DIR}/build)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Enable position-independent code (like -fPIC)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Compiler flags
if (SN_CONFIG_DEBUG)
    set(DEBUG_FLAGS -g -O0)
else()
    set(DEBUG_FLAGS -g -O1)
endif()


add_library(defs_interface INTERFACE)
target_compile_definitions(defs_interface INTERFACE BUILDING_SAFETYNET)
target_compile_definitions(defs_interface INTERFACE __SN_WIP_CALLS__)
target_compile_definitions(defs_interface INTERFACE __SN_DEBUG_CALLS__)

include(base_interface.cmake)
add_subdirectory(backend_api)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/libsafetynet_config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/include/libsafetynet_config.h
)

# Collect source files
file(GLOB C_SOURCES "${SRC_DIR}/*.c" "${SRC_DIR}/**/*.c")
#file(GLOB CXX_SOURCES "${SRC_DIR}/*.cpp")

add_library(safetynet_base INTERFACE)
target_include_directories(
        safetynet_base
        INTERFACE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)
target_sources(safetynet_base INTERFACE ${C_SOURCES})
target_link_libraries(safetynet_base INTERFACE defs_interface)
target_link_libraries(safetynet_base INTERFACE backend_api)

if (NOT SN_CONFIG_STATIC_ONLY)
# Shared library
add_library(safetynet_shared SHARED)
target_link_libraries(safetynet_shared PRIVATE safetynet_base)
target_include_directories(
        safetynet_shared
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(safetynet_shared PROPERTIES
    OUTPUT_NAME "safetynet"
    ARCHIVE_OUTPUT_DIRECTORY "${BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
)

if (WIN32)
    set_target_properties(safetynet_shared PROPERTIES
            PREFIX "lib"
            SUFFIX ".dll"
    )
endif ()

set(safetynet_out_lib safetynet_shared)

endif ()

if (SN_CONFIG_STATIC_ONLY)
# Static library
add_library(safetynet_static STATIC)
target_link_libraries(safetynet_static PRIVATE safetynet_base)
set_target_properties(safetynet_static PROPERTIES
    OUTPUT_NAME "safetynet"
    ARCHIVE_OUTPUT_DIRECTORY "${BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${out_path}"
)

target_include_directories(
        safetynet_static
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

set(safetynet_out_lib safetynet_static)

if (WIN32)
    set_target_properties(safetynet_static PROPERTIES
            PREFIX "lib"
            SUFFIX ".a"
    )
endif ()


# --- MRI Merge Setup ---
# Path to final output archive
set(MERGED_LIB_PATH $<TARGET_FILE:safetynet_static>)
set(BASE_LIB_PATH $<TARGET_FILE:backend_api>)

set(ORIGINAL_LIB $<TARGET_FILE:safetynet_static>)
set(TEMP_LIB "${ORIGINAL_LIB}.tmp")

# Path for temporary MRI script
set(MRI_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/safetynet_static.dir/merge.mri")

# Write MRI script (deferred so paths are expanded at build-time)
add_custom_command(TARGET safetynet_static PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E echo "CREATE ${MERGED_LIB_PATH}" > ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E echo "ADDLIB ${BASE_LIB_PATH}" >> ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E echo "ADDLIB ${TEMP_LIB}" >> ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E echo "SAVE" >> ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E echo "END" >> ${MRI_SCRIPT}
        COMMENT "Generating MRI merge script"
)

# Merge the base library into safetynet_static
add_custom_command(TARGET safetynet_static POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename ${ORIGINAL_LIB} ${TEMP_LIB}
        COMMAND ${CMAKE_AR} -M < ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E remove ${TEMP_LIB}
        COMMAND ${CMAKE_AR} -s $<TARGET_FILE:safetynet_static>
        COMMENT "Merging safetynet_base into safetynet_static"
)

add_custom_command(TARGET safetynet_static POST_BUILD
        COMMAND ${PLAT_OBJCOPY} -S --keep-symbols=${CMAKE_CURRENT_SOURCE_DIR}/public.sym $<TARGET_FILE:safetynet_static> $<TARGET_FILE:safetynet_static>
        COMMENT "Stripping static library symbols with objcopy"
)

endif ()

if (NOT SN_CONFIG_NO_TESTING_SUITE)
    message(STATUS "Loading Gtesting Component")
    find_package(GTest REQUIRED)
    enable_testing()
    add_subdirectory(Testing)
else ()
    message(STATUS "gTesting suite disabled")
endif ()