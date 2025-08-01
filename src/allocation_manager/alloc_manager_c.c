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
// Created by tete on 06/22/2025.
//
#include <string.h>

#include "alloc_manager_c.h"
#include "plat_allocators.h"
#include "sn_crash.h"

alloc_manager_m memman_new()
{
    alloc_manager_m self = plat_malloc(sizeof(alloc_manager_t));
    if (!self) sn_crash(SN_ERR_CATASTROPHIC);
    memset(self, 0, sizeof(alloc_manager_t));

    self->available_cache_slots = MEMMAN_MAX_CACHE_SLOTS;
}

void memman_destroy(alloc_manager_m self)
{
    if (!self) return;
    plat_free(self);
}

void memman_work(alloc_manager_m self)
{
}
