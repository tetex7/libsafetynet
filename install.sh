#!/usr/bin/env bash

if [ -e "/usr/bin/doas" ] ; then
    DOAS=doas
else
    DOAS=sudo
fi
PKG_NAME="libsafetynet"

rm -f ./${PKG_NAME}-*-1-x86_64.pkg.tar.zst

yes | ${DOAS} pacman -R ${PKG_NAME}
makepkg -f
yes | ${DOAS} pacman -U ./${PKG_NAME}-*-1-x86_64.pkg.tar.zst