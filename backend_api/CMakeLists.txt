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

file(GLOB BACKEND_C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c")

add_library(backend_api STATIC ${BACKEND_C_SOURCES})
target_link_libraries(backend_api PUBLIC base_interface)
target_include_directories(backend_api PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(backend_api PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(backend_api PRIVATE defs_interface)

set_target_properties(backend_api PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}
)