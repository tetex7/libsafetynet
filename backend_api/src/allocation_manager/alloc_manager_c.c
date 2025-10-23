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

#include "allocation_manager/alloc_manager_c.h"
#include "../../include/platform_independent/plat_allocators.h"
#include "sn_crash.h"

alloc_manager_m memman_new(plat_mutex_c mutex_ref)
{
    alloc_manager_m self = plat_malloc(sizeof(alloc_manager_t));
    if (!self) sn_crash(SN_ERR_CATASTROPHIC);
    memset(self, 0, sizeof(alloc_manager_t));

    self->available_cache_slots = MEMMAN_MAX_CACHE_SLOTS;
    self->global_memory_usage = MEMMAN_NO_ALLOC_LIMIT;
    self->cache_lock = 0;
    self->use_cache = 1;
    self->mutex_ref = mutex_ref;

    return self;
}

void memman_destroy(alloc_manager_m self)
{
    if (!self) return;
    plat_free(self);
}

// ReSharper disable once CppDFAConstantFunctionResult
static linked_list_entry_c memman_CacheAlgorithmWorker(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg)
{
    if (!generic_arg) return NULL;
    alloc_manager_m self_alloc_manager = (alloc_manager_m)generic_arg;

    if (ctx->_weight <= 10)
    {
        ctx->_weight = 0;
        ctx->cached = 1;
        for (size_t i = 0; i < MEMMAN_MAX_CACHE_SLOTS; i++)
        {
            if (self_alloc_manager->cache_list[i].value == NULL)
            {
                self_alloc_manager->cache_list[i].value = ctx;
                self_alloc_manager->cache_list[i].value->cached = 1;
                self_alloc_manager->cache_list[i].key = ctx->data;
                return NULL;
            }
            if (self_alloc_manager->cache_list[i].value->_weight < 10)
            {
                self_alloc_manager->cache_list[i].value = ctx;
                self_alloc_manager->cache_list[i].value->cached = 1;
                self_alloc_manager->cache_list[i].key = ctx->data;
                return NULL;
            }
        }
        return NULL;
    }

    return NULL;
}

void memman_work(alloc_manager_m self, linked_list_c list)
{
    if (!self) return;
    if (!list) return;
    plat_mutex_lock(self->mutex_ref);
    linked_list_forEach(list, &memman_CacheAlgorithmWorker, self);
    plat_mutex_unlock(self->mutex_ref);
}

linked_list_entry_c memman_TryCacheHit(alloc_manager_m self, void* key)
{
    if (!self) return MEMMAN_CACHE_MISS;
    if (!self->use_cache) return MEMMAN_CACHE_MISS;
    plat_mutex_lock(self->mutex_ref);
    for (size_t i = 0; i < MEMMAN_MAX_CACHE_SLOTS; i++)
    {
        if (!self->cache_list[i].key) continue;
        if (self->cache_list[i].key == key)
        {
            self->cache_list[i].value->_weight++;
            plat_mutex_unlock(self->mutex_ref);
            return self->cache_list[i].value;
        }
    }
    plat_mutex_unlock(self->mutex_ref);
    return MEMMAN_CACHE_MISS;
}

linked_list_entry_c memman_TryCacheHitById(alloc_manager_m self, uint16_t id)
{
    if (!self) return MEMMAN_CACHE_MISS;
    if (!self->use_cache) return MEMMAN_CACHE_MISS;
    plat_mutex_lock(self->mutex_ref);
    for (size_t i = 0; i < MEMMAN_MAX_CACHE_SLOTS; i++)
    {
        if (!self->cache_list[i].value) continue;
        if (self->cache_list[i].value->block_id == id)
        {
            plat_mutex_unlock(self->mutex_ref);
            return self->cache_list[i].value;
        }
    }
    plat_mutex_unlock(self->mutex_ref);
    return MEMMAN_CACHE_MISS;
}

void memman_cacheInvalidate(alloc_manager_m self, void* key)
{
    if (!self) return;
    plat_mutex_lock(self->mutex_ref);
    for (size_t i = 0; i < MEMMAN_MAX_CACHE_SLOTS; i++)
    {
        if (!self->cache_list[i].key) continue;
        if (self->cache_list[i].key == key)
        {
            self->cache_list[i].value->cached = 0;
            self->available_cache_slots++;
            memset(&self->cache_list[i], 0, sizeof(cache_pair_t));
        }
    }
    plat_mutex_unlock(self->mutex_ref);
}

void memman_cacheClear(alloc_manager_m self)
{
    if (!self) return;
    plat_mutex_lock(self->mutex_ref);
    for (size_t i = 0; i < MEMMAN_MAX_CACHE_SLOTS; i++)
    {
        if (!self->cache_list[i].key) continue;
        self->cache_list[i].value->cached = 1;
        self->available_cache_slots++;
        memset(&self->cache_list[i], 0, sizeof(cache_pair_t));
    }
    plat_mutex_unlock(self->mutex_ref);
}

SN_FLAG memman_tryCachePut(alloc_manager_m self, linked_list_entry_c entry)
{
    if (!self) return 0;
    if (self->cache_lock || !self->use_cache) return 0;
    if (self->available_cache_slots == 0) return 0;
    plat_mutex_lock(self->mutex_ref);

    for (size_t i = 0; i < MEMMAN_MAX_CACHE_SLOTS; i++)
    {
        if (self->cache_list[i].key == NULL)
        {
            self->available_cache_slots--;
            self->cache_list[i].key = entry->data;
            self->cache_list[i].value = entry;
            self->cache_list[i].value->cached = 1;
            plat_mutex_unlock(self->mutex_ref);
            return 1;
        }
    }
    sn_crash(SN_ERR_SYS_FAIL);
}

size_t memman_getGlobalMemoryUsage(alloc_manager_m self)
{
    if (!self) return 0;
    return self->global_memory_usage;
}

void memman_setGlobalMemoryUsage(alloc_manager_m self, size_t global_memory_usage)
{
    if (!self) return;
    self->global_memory_usage = global_memory_usage;
}

size_t memman_getAllocLimit(alloc_manager_m self)
{
    if (!self) return MEMMAN_NO_ALLOC_LIMIT;
    return self->alloc_limit;
}

void memman_setAllocLimit(alloc_manager_m self, size_t alloc_limit)
{
    if (!self) return;
    self->alloc_limit = alloc_limit;
}

SN_BOOL memman_canAllocateBasedOnLimit(alloc_manager_m self)
{
    if (!self) return SN_FALSE;
    if (memman_getAllocLimit(self) == MEMMAN_NO_ALLOC_LIMIT) return SN_TRUE;
    if (memman_getAllocLimit(self) <= memman_getGlobalMemoryUsage(self))
        return SN_FALSE;
    return SN_TRUE;
}

SN_BOOL memman_canAllocateBasedOnLimitAndSize(alloc_manager_m self, size_t size)
{
    if (!self) return SN_FALSE;
    if (memman_getAllocLimit(self) == MEMMAN_NO_ALLOC_LIMIT) return SN_TRUE;
    if (memman_getAllocLimit(self) <= memman_getGlobalMemoryUsage(self) + size)
        return SN_FALSE;
    return SN_TRUE;
}
