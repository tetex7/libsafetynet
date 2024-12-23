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


extern node_t* mem_list;

void* sn_malloc(size_t size)
{
    void* ret = malloc(size);

    if (!ret)
        return NULL;

    node_t* n = list_add(mem_list, ret);
    n->size = size;
    return ret;
}

void  sn_free(void* const ptr)
{
    if (!ptr)
        return;
    node_t* node = list_query(mem_list, ptr);
    if (node)
    {
        free(node->data);
        list_free(node);
    }
}

void* sn_register(void* const ptr)
{
    if (!ptr)
        return NULL;
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
        return 0;
    node_t* q = list_query(mem_list, ptr);
    if (q)
    {
        return q->size;
    }
    return 0;
}

uint64_t sn_query_tid(void *const ptr)
{
    node_t* q = list_query(mem_list, ptr);
    if (!q)
    {
        return q->tid;
    }
    return 0;
}

void* sn_register_size(void *ptr, size_t size)
{
    if (!ptr)
        return NULL;
    node_t* q = list_query(mem_list, ptr);
    if (!q)
    {
        node_t* temp = list_add(mem_list, ptr);
        temp->size = size;
        return ptr;
    }
    return ptr;
}
