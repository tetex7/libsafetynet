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
file(GLOB SN_FRONTEND_TEST_CPP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp")
add_executable(sn_frontend_test ${SN_FRONTEND_TEST_CPP_FILES})

set_target_output(sn_frontend_test ${BIN_DIR}/testing)

target_link_libraries(
        sn_frontend_test
        GTest::gtest_main
        sn_standard_test_base
)
target_compile_definitions(sn_frontend_test PUBLIC
    __SN_WIP_CALLS__
    #SN_ENABLE_CPP_NAMESPACE
)

target_link_libraries(sn_frontend_test ${safetynet_out_lib})

include(GoogleTest)
gtest_discover_tests(sn_frontend_test)