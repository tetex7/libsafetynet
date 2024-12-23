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

#include "libsafetynet.h"
#include "pri_list.h"
#include "_pri_api.h"



extern node_t* mem_list;
pthread_mutex_t last_error_mutex = PTHREAD_MUTEX_INITIALIZER;

void* sn_malloc(size_t size)
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
    return NULL;
}

sn_error_codes_e sn_get_last_error()
{
    return error_code;
}

void sn_reset_last_error()
{
    pthread_mutex_lock(&last_error_mutex);
    error_code = SN_ERR_OK;
    pthread_mutex_unlock(&last_error_mutex);
}
