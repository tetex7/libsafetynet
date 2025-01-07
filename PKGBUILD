#
# Copyright (C) 2024  tete
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

# Maintainer: tete <tetex7@outlook.com>
pkgname=libsafetynet
pkgver=1.2.1
pkgrel=1
pkgdesc='Passive garbage collector for C and C++'
url='www.github.com/tetex7/libsafetynet'
provides=()
arch=('x86_64')
PACKAGER="TRS"
license=('GPL3')
makedepends=('gcc' 'binutils' 'valgrind')
depends=('glibc')
options=('!strip' 'docs' 'libtool' 'staticlibs' 'emptydirs' 'zipman' '!purge' '!debug' '!lto')

prepare() {
cd ..
echo $PWD
make clean
}

build () {
    cd ..
    if [ -z "${PK_DEBUG}" ]; then
        make DEBUG=0 VER=${pkgver}
    else
        make DEBUG=${PK_DEBUG} VER=${pkgver}
    fi

    ./test.sh
    if [ ${?} == 0 ]; then
        echo "Test passed no memory leaks"
    elif [ ${?} == 44 ]; then
        echo "Test Compilation error"
        exit ${?}
    else
        echo "Memory error detected: see valgrind_output.log"
        exit 1
    fi

}

package() {
    cd ..
    minverN=$(echo "${pkgver}" | head -c 1)
    eminverN=$(echo "${pkgver}" | head -c 3)
    install -Dm755 ./build/libsafetynet.so "${pkgdir}/usr/lib/libsafetynet.so.${pkgver}"
    install -Dm644 "./include/libsafetynet.h" "${pkgdir}/usr/include/libsafetynet.h"

    for i in $(ls ./manpages); do
        install -Dm644 "./manpages/$i" "${pkgdir}/usr/man/man3/${i}"
    done


    ln -s "/usr/lib/${pkgname}.so.${pkgver}" "${pkgdir}/usr/lib/${pkgname}.so"
    ln -s "/usr/lib/${pkgname}.so.${pkgver}" "${pkgdir}/usr/lib/${pkgname}.so.${minverN}"
    ln -s "/usr/lib/${pkgname}.so.${pkgver}" "${pkgdir}/usr/lib/${pkgname}.so.${eminverN}"
    
}
