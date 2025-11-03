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

option(SN_STAND_ALONE_PACKAGE_BUNDLE "Produces a simple bundle package Default Packaging style" ON)
option(SN_PACKAGE_DEB "Produces a simple deb" OFF)
option(SN_WIN32_NON_BUNDLE_OVERRIDE "Allows the production of non bundle style packages for Windows" OFF)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

set(CPACK_ARCHIVE_TGZ_OPTIONS "--owner=0;--group=0;--numeric-owner")
set(CPACK_ARCHIVE_ZIP_OPTIONS "-X")

string(TOLOWER "${CMAKE_SYSTEM_NAME}" PACK_SYS)
set(CPACK_PACKAGE_FILE_NAME
        "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${PACK_SYS}"
)

if(SN_CONFIG_STATIC_ONLY)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-static")
endif()

if (WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif (SN_PACKAGE_DEB AND NOT SN_STAND_ALONE_PACKAGE_BUNDLE)
    set(CPACK_GENERATOR "DEB")
else ()
    set(CPACK_GENERATOR "TGZ")
endif ()

if(SN_STAND_ALONE_PACKAGE_BUNDLE OR (WIN32 AND NOT SN_WIN32_NON_BUNDLE_OVERRIDE))
    # only install headers + libs for bundle
    install(TARGETS ${safetynet_out_lib}
            ARCHIVE DESTINATION .
            LIBRARY DESTINATION .
            RUNTIME DESTINATION .
    )
    install(DIRECTORY include/ DESTINATION .)

else ()
    if (SN_PACKAGE_DEB)
        message(WARNING "SN_PACKAGE_DEB is work in progress")
        if (NOT DEFINED CPACK_DEBIAN_PACKAGE_MAINTAINER)
            message(FATAL_ERROR "Please fill in CPACK_DEBIAN_PACKAGE_MAINTAINER")
        endif ()
        set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
    endif ()

    if (IS_BSD)
        set(SN_BASE_INSTALL_PREFIX usr/local)
    else ()
        set(SN_BASE_INSTALL_PREFIX usr)
    endif ()
    set(SN_INSTALL_PREFIX /${SN_BASE_INSTALL_PREFIX})

    install(TARGETS ${safetynet_out_lib}
            ARCHIVE DESTINATION ${SN_BASE_INSTALL_PREFIX}/lib
            LIBRARY
            NAMELINK_COMPONENT Runtime
            DESTINATION ${SN_BASE_INSTALL_PREFIX}/lib
            RUNTIME DESTINATION ${SN_BASE_INSTALL_PREFIX}/bin
    )
    install(DIRECTORY include/ DESTINATION ${SN_BASE_INSTALL_PREFIX}/include)

    configure_file(libsafetynet.pc.in
            "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/libsafetynet.pc"
            @ONLY)

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/libsafetynet.pc"
            DESTINATION ${SN_BASE_INSTALL_PREFIX}/lib/pkgconfig)
endif()
    include(CPack)