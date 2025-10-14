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
// Created by tete on 06/15/2025.
//
#include "_pri_api.h"

#include <stdlib.h>
#include <string.h>

#include "plat_allocators.h"


SN_PUB_API_OPEN void* sn_malloc(size_t size)
{
    if (!size)
    {
        sn_error(SN_ERR_BAD_SIZE, NULL);
    }

    void* pr = plat_malloc(size);

    if (!pr)
    {
        sn_error(SN_ERR_BAD_ALLOC, NULL);
    }

//The config macro does not fully conform to what we're doing here lol
#ifdef SN_CONFIG_SANITIZE_MEMORY_ON_FREE
    memset(pr, 0, size);
#endif
    memory_manager->global_memory_usage += size;
    linked_list_push(mem_list, pr, size, plat_getTid());

    return pr;
}



SN_PUB_API_OPEN void sn_free(void* const ptr)
{
    if (!ptr)
    {
        sn_error(SN_ERR_NULL_PTR);
    }

    linked_list_entry_c entry = memman_TryCacheHit(memory_manager, ptr);
    if (entry == MEMMAN_CACHE_MISS)
    {
        entry = linked_list_getByPtr(mem_list, ptr);
        if (!entry)
            sn_error(SN_ERR_NO_ADDER_FOUND);
    }

#ifdef SN_CONFIG_SANITIZE_MEMORY_ON_FREE
    memset(entry->data, 0, entry->size);
#endif
    memory_manager->global_memory_usage -= entry->size;
    memman_cacheInvalidate(memory_manager, ptr);
    plat_free(linked_list_entry_getData(entry));

    linked_list_removeEntryByPtr(mem_list, ptr);
}

SN_PUB_API_OPEN void* sn_calloc(size_t num, size_t size)
{
    if (!size)
    {
        sn_error(SN_ERR_BAD_SIZE, NULL);
    }

    void* pr = plat_calloc(num, size);

    if (!pr)
    {
        sn_error(SN_ERR_BAD_ALLOC, NULL);
    }
    memory_manager->global_memory_usage += (size * num);
    linked_list_push(mem_list, pr, size, plat_getTid());

    return pr;
}

SN_PUB_API_OPEN void* sn_realloc(void* ptr, size_t new_size)
{
    if (!ptr)
    {
        sn_error(SN_ERR_NULL_PTR, NULL);
    }

    linked_list_entry_c entry = linked_list_getByPtr(mem_list, ptr);
    if (!entry)
    {
        sn_error(SN_ERR_NO_ADDER_FOUND, NULL);
    }

    if (!new_size)
    {
        sn_error(SN_ERR_BAD_SIZE, NULL);
    }

    void* new_ptr = plat_realloc(ptr, new_size);

    if (!new_ptr)
    {
        sn_error(SN_ERR_BAD_ALLOC, NULL);
    }
    memory_manager->global_memory_usage -= entry->size;
    memory_manager->global_memory_usage += new_size;

    linked_list_entry_setData(entry, new_ptr);
    linked_list_entry_setSize(entry, new_size);
    return new_ptr;
}

SN_PUB_API_OPEN void* sn_malloc_pre_initialized(size_t size, uint8_t initial_byte_value)
{
    void* ptr = sn_malloc(size);
    if (!ptr) return ptr;

    memset(ptr, initial_byte_value, size);
    return ptr;
}
