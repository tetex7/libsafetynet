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


#define MEMMAN_MAX_CACHE_SLOTS 5

#define MEMMAN_CACHE_MISS NULL
#define MEMMAN_NO_ALLOC_LIMIT 0

typedef struct cache_pair_s
{
    void* key;
    linked_list_entry_c value;
} cache_pair_t;

typedef struct alloc_manager_s
{
    cache_pair_t cache_list[MEMMAN_MAX_CACHE_SLOTS];
    uint8_t available_cache_slots;
    SN_BOOL cache_lock;
    SN_BOOL use_cache;

    size_t alloc_limit;
    size_t global_memory_usage;

    plat_mutex_c mutex_ref; //A reference to a pre-existing mutex Therefore not managed by this object
} *alloc_manager_m, alloc_manager_t;


alloc_manager_m memman_new(plat_mutex_c mutex_ref);

void memman_destroy(alloc_manager_m self);
void memman_work(alloc_manager_m self, linked_list_c list);

linked_list_entry_c memman_TryCacheHit(alloc_manager_m self, void* key);
linked_list_entry_c memman_TryCacheHitById(alloc_manager_m self, sn_block_id_t id);
void memman_cacheInvalidate(alloc_manager_m self, void* key);
void memman_cacheClear(alloc_manager_m self);
SN_FLAG memman_tryCachePut(alloc_manager_m self, linked_list_entry_c entry);

size_t memman_getGlobalMemoryUsage(alloc_manager_m self);
void memman_setGlobalMemoryUsage(alloc_manager_m self, size_t global_memory_usage);

size_t memman_getAllocLimit(alloc_manager_m self);
void memman_setAllocLimit(alloc_manager_m self, size_t alloc_limit);

SN_BOOL memman_canAllocateBasedOnLimit(alloc_manager_m self);
SN_BOOL memman_canAllocateBasedOnLimitAndSize(alloc_manager_m self, size_t size);

#endif //ALLOC_MANAGER_C_H
