#!/usr/bin/env bash
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

set -e

readonly PAK_NAME="libsafetynet-deployment-package"

rm -rf ./${PAK_NAME}

cp -rv ./include ./${PAK_NAME}
cp -v ./build/libsafetynet.* ./${PAK_NAME}

readonly GIT_COMMIT_HASH=$(git rev-parse --short HEAD)
readonly GIT_BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)

echo "Built: $(date)" > "${PAK_NAME}/build-info.txt"
echo "Commit: ${GIT_COMMIT_HASH}" >> "${PAK_NAME}/build-info.txt"
echo "Branch: ${GIT_BRANCH_NAME}" >> "${PAK_NAME}/build-info.txt"
echo "Generated ${PAK_NAME}/build-info.txt"