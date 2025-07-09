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
#pragma once

// This file is a work in progress allocation manager
// To reimplement functions requiring more advanced state keeping
// e.g. sn_set_alloc_limit

#ifndef ALLOC_MANAGER_C_H
#define ALLOC_MANAGER_C_H
#include "libsafetynet.h"
#include "linked_list_c.h"

enum
{
    MEMMAN_CUS_MALLOC,
    MEMMAN_CUS_REALLOC,
    MEMMAN_CUS_CALLOC,
    MEMMAN_CUS_FREE,
};

#define MEMMAN_MAX_CACHE_SLOTS 5
#define MEMMAN_CUS_ENUM_LEN 4

typedef struct cache_pair_s
{
    void* key;
    linked_list_entry_c value;
} cache_pair_t;

typedef struct alloc_manager_s
{
    struct
    {
        sn_memalloc_call_t cus_malloc;
        sn_free_call_t cus_free;
        sn_calloc_call_t cus_calloc;
        sn_realloc_call_t cus_realloc;
    } allocators;
    cache_pair_t cache_list[MEMMAN_MAX_CACHE_SLOTS];
    uint8_t available_cache_slots;


    size_t alloc_limit;
    size_t global_memory_usage;
} *alloc_manager_m, alloc_manager_t;


alloc_manager_m memman_new();

void memman_destroy(alloc_manager_m self);
void memman_work(alloc_manager_m self);

#endif //ALLOC_MANAGER_C_H
