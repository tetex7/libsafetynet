/*
 * Copyright (C) 2024  Tete
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

#define __SN_WIP_CALLS__
#include "pri_list.h"
#include "_pri_api.h"



extern node_t* mem_list;
pthread_mutex_t last_error_mutex = PTHREAD_MUTEX_INITIALIZER;


/**
 *
 * @param size Size in bytes to be allocated
 * @return a block Allocated or null on failure use sn_get_last_error() for more info
 */
SN_PUB_API_OPEN
void* sn_malloc(const size_t size)
{
    if (!size)
    {
        sn_set_last_error(SN_ERR_BAD_SIZE);
        return NULL;
    }

    void* ret = malloc(size);

    if (!ret)
    {
        sn_set_last_error(SN_ERR_BAD_ALLOC);
        return NULL;
    }

    node_t* n = list_add(mem_list, ret);
    n->size = size;
    return ret;
}

SN_PUB_API_OPEN
void  sn_free(void* const ptr)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return;
    }

    node_t* node = list_query(mem_list, ptr);
    if (node)
    {
        free(node->data);
        list_free(node);
        return;
    }
    sn_set_last_error(SN_WARN_DUB_FREE);
}

SN_PUB_API_OPEN
void* sn_register(void* const ptr)
{
    sn_set_last_error(SN_ERR_NO_SIZE);
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return NULL;
    }
    node_t* q = list_query(mem_list, ptr);
    if (!q)
    {
        list_add(mem_list, ptr);
        return ptr;
    }
    return ptr;
}

SN_PUB_API_OPEN
size_t sn_query_size(void* const ptr)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return 0;
    }
    node_t* q = list_query(mem_list, ptr);
    if (q)
    {
        if(!q->size)
            sn_set_last_error(SN_ERR_NO_SIZE);
        return q->size;
    }
    sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
    return 0;
}

SN_PUB_API_OPEN
uint64_t sn_query_tid(void *const ptr)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return 0;
    }

    node_t* q = list_query(mem_list, ptr);
    if (q)
    {
        return q->tid;
    }
    sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
    return 0;
}

SN_PUB_API_OPEN
void* sn_register_size(void *ptr, size_t size)
{
    if (!size)
    {
        sn_set_last_error(SN_ERR_BAD_SIZE);
        return NULL;
    }

    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return NULL;
    }
    node_t* q = list_query(mem_list, ptr);
    if (!q)
    {
        node_t* temp = list_add(mem_list, ptr);
        temp->size = size;
        return ptr;
    }
    sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
    return NULL;
}

SN_PUB_API_OPEN
sn_error_codes_e sn_get_last_error()
{
    return error_code;
}

SN_PUB_API_OPEN
void sn_reset_last_error()
{
    pthread_mutex_lock(&last_error_mutex);
    error_code = SN_ERR_OK;
    pthread_mutex_unlock(&last_error_mutex);
}

static const char* const human_readable_messages[] = {
    [SN_ERR_OK] = "everything is AOK",
    [SN_ERR_NULL_PTR] = "Nullprinter provided to function",
    [SN_ERR_NO_SIZE] = "no size Metadata Provided or available",
    [SN_ERR_BAD_SIZE] = "Invalid size provided",
    [SN_ERR_BAD_ALLOC] = "libc malloc Returned null",
    [SN_ERR_NO_ADDER_FOUND] = "no adder provided or available",
    [SN_WARN_DUB_FREE] = "Possible double free but it could not be in the registry though"
};


/**
 * Provide you a human-readable error message
 * @param err The error code
 * @return A pointer to the string containing the error message (Do not manipulate the string Treat it as immutable)
 */
SN_PUB_API_OPEN const char* const SN_API_PREFIX(get_error_msg)(sn_error_codes_e err)
{
    switch (err)
    {
        case SN_ERR_OK: return human_readable_messages[SN_ERR_OK];
        case SN_ERR_NULL_PTR: return human_readable_messages[SN_ERR_NULL_PTR];
        case SN_ERR_BAD_SIZE: return human_readable_messages[SN_ERR_BAD_SIZE];
        case SN_ERR_BAD_ALLOC: return human_readable_messages[SN_ERR_BAD_ALLOC];
        case SN_ERR_NO_ADDER_FOUND: return human_readable_messages[SN_ERR_NO_ADDER_FOUND];
        case SN_WARN_DUB_FREE: return human_readable_messages[SN_WARN_DUB_FREE];
        default: return "Unknown error";
    }
}

SN_PUB_API_OPEN const sn_mem_metadata_t* SN_API_PREFIX(query_metadata)(void *ptr)
{

    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return NULL;
    }
    node_t* q = list_query(mem_list, ptr);
    if (q)
    {
        return (sn_mem_metadata_t*)&q->data;
    }
    sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
    return NULL;
}

/**
 * @brief Checks if a block is being tracked by the safety net system;
 * @param ptr Pointer to the block of memory
 * @return A flag to which it exists (a bool)
 */
SN_PUB_API_OPEN SN_FLAG SN_API_PREFIX(is_tracked_block)(const void* const ptr)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return SN_FLAG_UNSET;
    }
    node_t* q = list_query(mem_list, ptr);
    if (q)
    {
        return SN_FLAG_SET;
    }
    return SN_FLAG_UNSET;
}