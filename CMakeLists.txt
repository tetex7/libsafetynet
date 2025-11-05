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

project(libsafetynet VERSION ${SN_CONFIG_VERSION} LANGUAGES C)

include(BuildOptions.cmake)

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

if(SN_CONFIG_ENABLE_STACK_TRACE)
    if (WIN32)
        message(WARNING "If you are cross compiling on Linux please set PKG_CONFIG_EXECUTABLE to a Cross compiler wrapper")
    endif ()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(BACKTRACE REQUIRED libbacktrace)
    message(STATUS "Stacktrace component enabled")
    message(STATUS "Using backtrace for stacktrace")
    include_directories(${BACKTRACE_INCLUDES})
    link_libraries(${BACKTRACE_LIBRARIES})
endif ()


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
set(CMAKE_PLATFORM_NO_VERSIONED_SONAME  ON)

set_target_properties(safetynet_shared PROPERTIES
        VERSION ${PROJECT_VERSION}        # Full version, makes the file libsafetynet.so.1.2.0
        SOVERSION ${PROJECT_VERSION_MAJOR}          # ABI version, makes SONAME libsafetynet.so.1
)



if (WIN32)
    set_target_properties(safetynet_shared PROPERTIES
            PREFIX "lib"
            SUFFIX ".dll"
    )
endif ()


if (SN_CONFIG_SHARED_LIBRARY_POST_PROCESSING)
    add_custom_target(safetynet_shared_post_proc ALL
            DEPENDS safetynet_shared
            COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/scripts/smart_strip.py --silent --objcopy ${PLAT_OBJCOPY} --nm ${PLAT_NM} $<TARGET_FILE:safetynet_shared>
            COMMENT "Finalizing and post processing shared library"
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
add_custom_target(gen_mri_merge_script
        COMMAND ${CMAKE_COMMAND} -E echo "CREATE ${MERGED_LIB_PATH}" > ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E echo "ADDLIB ${BASE_LIB_PATH}" >> ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E echo "ADDLIB ${TEMP_LIB}" >> ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E echo "SAVE" >> ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E echo "END" >> ${MRI_SCRIPT}
        BYPRODUCTS ${MRI_SCRIPT}
        COMMENT "Generating MRI merge script"
)

# Merge the base library into safetynet_static
add_custom_target(safetynet_static_post_proc ALL
        COMMAND ${CMAKE_COMMAND} -E rename ${ORIGINAL_LIB} ${TEMP_LIB}
        COMMAND ${CMAKE_AR} -M < ${MRI_SCRIPT}
        COMMAND ${CMAKE_COMMAND} -E remove ${TEMP_LIB}
        COMMAND ${CMAKE_RANLIB} $<TARGET_FILE:safetynet_static>
        COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/scripts/smart_strip.py --silent --objcopy ${PLAT_OBJCOPY} --nm ${PLAT_NM} $<TARGET_FILE:safetynet_static>
        DEPENDS safetynet_static
        DEPENDS gen_mri_merge_script
        COMMENT "Finalizing and post processing static library"
)

#add_custom_command(TARGET safetynet_static POST_BUILD
#        COMMAND ${PLAT_OBJCOPY} -S --keep-symbols=${CMAKE_CURRENT_SOURCE_DIR}/public.sym $<TARGET_FILE:safetynet_static> $<TARGET_FILE:safetynet_static>
#        COMMENT "Stripping static library symbols with objcopy"
#)

endif ()

if (NOT SN_CONFIG_NO_TESTING_SUITE)
    message(STATUS "Loading Gtesting Component")
    find_package(GTest REQUIRED)
    enable_testing()
    add_subdirectory(Testing)
else ()
    message(STATUS "gTesting suite disabled")
endif ()

if (SN_USE_CPACK_PACKAGING)
    message(STATUS "Loading Cpack Component")
    include(Packaging.cmake)
else ()
    message(STATUS "Cpack Component disabled")
    set(CMAKE_SKIP_INSTALL_RULES ON)
endif ()