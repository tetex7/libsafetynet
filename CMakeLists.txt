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

include(BuildOptions.cmake)

project(safetynet VERSION ${SN_CONFIG_STR_VERSION} LANGUAGES C)

#enable_testing()

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

if(SN_CONFIG_STR_VERSION)
    set(VER_FLAG_S "${SN_CONFIG_STR_VERSION}")
else()
    set(VER_FLAG_S "dev")
endif()

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
target_link_libraries(safetynet_base INTERFACE backend_api)
target_compile_definitions(safetynet_base INTERFACE BUILDING_SAFETYNET)
target_compile_definitions(safetynet_base INTERFACE __SN_WIP_CALLS__)

if (NOT WIN32 OR NOT SN_CONFIG_STATIC_ONLY)
# Shared library
add_library(safetynet_shared SHARED empty.c)
target_link_libraries(safetynet_shared PUBLIC safetynet_base)
set_target_properties(safetynet_shared PROPERTIES
    OUTPUT_NAME "safetynet"
    LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}
)
endif ()

if (SN_CONFIG_STATIC_ONLY OR WIN32)
# Static library
add_library(safetynet_static STATIC empty.c)
target_link_libraries(safetynet_shared PRIVATE safetynet_base)
set_target_properties(safetynet_static PROPERTIES
    OUTPUT_NAME "safetynet"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
)

add_custom_command(TARGET safetynet_static POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -S --keep-symbols=${CMAKE_CURRENT_SOURCE_DIR}/public.sym $<TARGET_FILE:safetynet_static> $<TARGET_FILE:safetynet_static>
        COMMENT "Stripping static library symbols with objcopy"
)

endif ()

if (NOT WIN32)
# Optional: add test rule
#add_custom_target(mem_test
#    COMMAND ${CMAKE_SOURCE_DIR}/test.sh
#    DEPENDS safetynet_shared
#    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#)
endif ()