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
// Created by tete on 12/02/2025.
//

#include "linked_list_c.h"
#include "_pri_api.h"


SN_API sn_mem_stack_c sn_mem_stack_new()
{
    linked_list_c linked = linked_list_new();

    return (sn_mem_stack_c)linked;
}

size_t sn_mem_stack_getSize(sn_mem_stack_c self)
{
    if (!self)
    {
        sn_error(SN_ERR_NULL_PTR, 0);
    }

    return linked_list_getSize((linked_list_c)self);
}

SN_API void* sn_mem_stack_peek(sn_mem_stack_c self)
{
    if (!self)
    {
        sn_error(SN_ERR_NULL_PTR, NULL);
    }

    void* ptr = linked_list_entry_getData(linked_list_peek((linked_list_c)self));
    return ptr;
}

size_t sn_mem_stack_peekSizeOfNextPop(sn_mem_stack_c self)
{
    if (!self)
    {
        sn_error(SN_ERR_NULL_PTR, 0);
    }

    linked_list_entry_c entry = ((linked_list_c)self)->lastEntry;

    if (!entry)
    {
        sn_error(SN_ERR_NO_ADDER_FOUND, 0);
    }

    return linked_list_entry_getSize(entry);
}

void* sn_mem_stack_getIndex(sn_mem_stack_c self, size_t index)
{
    if (!self)
    {
        sn_error(SN_ERR_NULL_PTR, NULL);
    }

    if (index >= linked_list_getSize((linked_list_c)self))
    {
        sn_error(SN_ERR_BAD_SIZE, NULL);
    }

    linked_list_entry_c entry = linked_list_getByIndex((linked_list_c)self, index);

    if (!entry)
    {
        sn_error(SN_ERR_NO_ADDER_FOUND, NULL);
    }

    void* ptr = linked_list_entry_getData(entry);
    return ptr;
}

SN_API void* sn_mem_stack_pop(sn_mem_stack_c self)
{
    if (!self)
    {
        sn_error(SN_ERR_NULL_PTR, NULL);
    }

    void* ptr = linked_list_entry_getData(linked_list_peek((linked_list_c)self));
    linked_list_pop((linked_list_c)self);
    return ptr;
}

SN_API void sn_mem_stack_push(sn_mem_stack_c self, void* data)
{
    if (!self)
    {
        sn_error(SN_ERR_NULL_PTR);
    }

    linked_list_push((linked_list_c)self, data, 0, 0);
}

void sn_mem_stack_npush(sn_mem_stack_c self, void* data, size_t size)
{
    if (!self)
    {
        sn_error(SN_ERR_NULL_PTR);
    }

    linked_list_push((linked_list_c)self, data, 0, size);
}

#ifdef SN_ON_CLANG
SN_API SN_OVERLOAD_ELIGIBLE void sn_mem_stack_push(sn_mem_stack_c self, void* data, size_t size)
{
    sn_mem_stack_npush(self, data, size);
}
#endif

typedef struct
{
    void* generic_arg;
    void** ret_out;
    sn_mem_stack_for_each_worker_f stack_worker;
} garg_t;

static linked_list_entry_c mem_stack_for_each_worker(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg)
{
    garg_t* g = (garg_t*)generic_arg;

    (*g->ret_out) = g->stack_worker(ctx->data, index, g->generic_arg);
    return (*g->ret_out);
}

SN_API void* sn_mem_stack_forEach(sn_mem_stack_c self, sn_mem_stack_for_each_worker_f worker, void* generic_arg)
{
    if (!self || !worker)
    {
        sn_error(SN_ERR_NULL_PTR, NULL);
    }
    void* r = NULL;

    linked_list_forEach((linked_list_c)self, &mem_stack_for_each_worker, &((garg_t){
        generic_arg,
        &r,
        worker
    }));

    return r;
}

SN_API void sn_mem_stack_destroy(sn_mem_stack_c self)
{
    if (!self)
    {
        sn_error(SN_ERR_NULL_PTR);
    }
    linked_list_destroy((linked_list_c)self);
}