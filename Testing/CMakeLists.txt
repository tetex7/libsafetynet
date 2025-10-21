#
# Copyright (C) 2025  tetex7
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
enable_language(CXX)

add_library(sn_standard_test_base INTERFACE)
target_link_libraries(sn_standard_test_base INTERFACE base_interface)
target_compile_options(sn_standard_test_base INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-O0 -g -Wall -Wextra -Werror -Wpedantic -fno-strict-aliasing>)
target_compile_features(sn_standard_test_base INTERFACE
        cxx_std_23
)

add_subdirectory(frontend_api_tests)