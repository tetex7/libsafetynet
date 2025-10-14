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
// Created by tete on 06/18/2025.
//
#include "libsafetynet.h"
#include "_pri_api.h"
#include <string.h>

SN_PUB_API_OPEN SN_MSG_DEPRECATED("unsafe due to lack of The definition of size") void* sn_register(void* const ptr)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    linked_list_push(mem_list, ptr, 0, plat_getTid());
    return ptr;
}

SN_PUB_API_OPEN size_t sn_query_size(void* const ptr)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    if (!ptr)
    {
        sn_error(SN_ERR_NULL_PTR, 0);
    }

    linked_list_entry_c entry = linked_list_getByPtr(mem_list, ptr);
    if (!entry)
    {
        sn_error(SN_ERR_NO_ADDER_FOUND, 0);
    }

    return linked_list_entry_getSize(entry);
}

SN_PUB_API_OPEN sn_tid_t sn_query_tid(void* const ptr)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    if (!ptr)
    {
        sn_error(SN_ERR_NULL_PTR, 0);
    }

    linked_list_entry_c entry = memman_TryCacheHit(memory_manager, ptr);

    if (entry == MEMMAN_CACHE_MISS)
    {
        entry = linked_list_getByPtr(mem_list, ptr);
        if (!entry)
            sn_error(SN_ERR_NO_ADDER_FOUND, 0);
    }

    return linked_list_entry_getTid(entry);
}

SN_PUB_API_OPEN void* sn_register_size(void* ptr, size_t size)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    if (!ptr)
    {
        sn_error(SN_ERR_NULL_PTR, NULL);
    }

    if (!size)
    {
        sn_error(SN_ERR_BAD_SIZE, NULL);
    }

    if (linked_list_hasPtr(mem_list, ptr)) return ptr;

    linked_list_push(mem_list, ptr, size, plat_getTid());
    return ptr;
}

SN_PUB_API_OPEN SN_FLAG sn_is_tracked_block(const void* const ptr)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    if (!ptr)
    {
        sn_error(SN_ERR_NULL_PTR, 0);
    }

    return linked_list_hasPtr(mem_list, (void*)ptr);
}

SN_PUB_API_OPEN void sn_set_block_id(void* block, uint16_t id)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    if (!block)
    {
        sn_error(SN_ERR_NULL_PTR);
    }

    if (id <= 20)
    {
        sn_error(SN_ERR_BAD_BLOCK_ID);
    }

    linked_list_entry_c entry = memman_TryCacheHit(memory_manager, block);

    if (entry == MEMMAN_CACHE_MISS)
    {
        entry = linked_list_getByPtr(mem_list, block);
        if (!entry)
            sn_error(SN_ERR_NO_ADDER_FOUND);
    }

    linked_list_entry_setBlockId(entry, id);
}

SN_PUB_API_OPEN uint16_t sn_get_block_id(void* block)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    if (!block)
    {
        sn_error(SN_ERR_NULL_PTR, 0);
    }

    linked_list_entry_c entry = memman_TryCacheHit(memory_manager, block);

    if (entry == MEMMAN_CACHE_MISS)
    {
        entry = linked_list_getByPtr(mem_list, block);
        if (!entry)
            sn_error(SN_ERR_NO_ADDER_FOUND, 0);
    }

    return linked_list_entry_getBlockId(entry);
}

SN_PUB_API_OPEN void* sn_query_block_id(uint16_t id)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    if (!id)
    {
        sn_error(SN_ERR_BAD_BLOCK_ID, NULL);
    }

    linked_list_entry_c entry = memman_TryCacheHitById(memory_manager, id);

    if (entry == MEMMAN_CACHE_MISS)
    {
        entry = linked_list_getById(mem_list, id);
        if (!entry)
            sn_error(SN_ERR_NO_ADDER_FOUND, NULL);
    }

    return linked_list_entry_getData(entry);
}

SN_PUB_API_OPEN const sn_mem_metadata_t* sn_query_metadata(void* ptr)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    if (!ptr)
    {
        sn_error(SN_ERR_NULL_PTR, NULL);
    }

    linked_list_entry_c entry = memman_TryCacheHit(memory_manager, ptr);

    if (entry == MEMMAN_CACHE_MISS)
    {
        entry = linked_list_getByPtr(mem_list, ptr);
        if (!entry)
            sn_error(SN_ERR_NO_ADDER_FOUND, NULL);
    }

    //Yes very spooky, but it's a known good view into memory
    return (sn_mem_metadata_t*)&entry->data;
}

SN_PUB_API_OPEN const sn_mem_metadata_t* sn_query_static_metadata(void* ptr)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    const sn_mem_metadata_t* mem_metadata = sn_query_metadata(ptr);
    if (!mem_metadata) return NULL;

    sn_mem_metadata_t* ret_copy_meta = sn_malloc(sizeof(sn_mem_metadata_t));
    if (!ret_copy_meta) return NULL;

    memcpy(ret_copy_meta, mem_metadata, sizeof(sn_mem_metadata_t));
    return ret_copy_meta;
}



void sn_set_alloc_limit(size_t limit)
{
    memory_manager->alloc_limit = limit;
}

typedef struct
{
    sn_tid_t tid;
    size_t* out_size;
} _pri_tid_size_t; // NOLINT(*-reserved-identifier)

static linked_list_entry_c search_for_tid(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg)
{
    if (ctx->tid == ((_pri_tid_size_t*)generic_arg)->tid)
    {
        (*((_pri_tid_size_t*)generic_arg)->out_size) += ctx->size;
    }
    return NULL;
}

SN_PUB_API_OPEN
size_t sn_query_thread_memory_usage(sn_tid_t tid)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    size_t siz = 0;
    linked_list_forEach(mem_list, &search_for_tid, &(_pri_tid_size_t){
        tid,
        &siz
    });

    return siz;
}

size_t sn_query_total_memory_usage()
{
    return memory_manager->global_memory_usage;
}

SN_PUB_API_OPEN uint64_t sn_calculate_checksum(void* block)
{
    memman_work(memory_manager, mem_list); // Let's Steal some CPU time
    if (!block)
    {
        sn_error(SN_ERR_NULL_PTR, 0);
    }

    linked_list_entry_c entry = memman_TryCacheHit(memory_manager, block);

    if (entry == MEMMAN_CACHE_MISS)
    {
        entry = linked_list_getByPtr(mem_list, block);
        if (!entry)
            sn_error(SN_ERR_NO_ADDER_FOUND, 0);
    }

    const size_t size = linked_list_entry_getSize(entry);

    if (!entry->size)
    {
        sn_error(SN_ERR_BAD_SIZE, 0);
    }

    const uint8_t* const data = block;

    if (size == 1)
    {
        return *data ^ 0xFF;
    }

    uint64_t checksum = 0;
    const uint8_t first_value = *data;
    const uint8_t last_value = data[size - 1];
    const uint8_t mid_value = data[size/2];
    const uint8_t mixer = ~(first_value ^ last_value) ^ mid_value;
    for (size_t i = 0; i < size; i++)
    {
        checksum = ((((data[i] ^ mixer) + mid_value) * last_value) + (*(uint8_t*)&i)) ^ checksum;
    }
    return (checksum);
}

