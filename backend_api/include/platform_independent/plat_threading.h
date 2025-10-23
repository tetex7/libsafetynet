/*
 * Copyright (C) 2025  Tetex7
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

//
// Created by tete on 06/16/2025.
//
#pragma once

#ifndef PLAT_THREADING_H
#define PLAT_THREADING_H
#include <stdint.h>

typedef struct plat_mutex_s* plat_mutex_c, plat_mutex_t;

plat_mutex_c plat_mutex_new();
void plat_mutex_lock(plat_mutex_c self);
void plat_mutex_unlock(plat_mutex_c self);
void plat_mutex_destroy(plat_mutex_c self);

uint64_t plat_getTid();

#endif //PLAT_THREADING_H
