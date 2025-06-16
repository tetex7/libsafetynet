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


SN_PUB_API_OPEN void* sn_malloc(size_t size)
{
    if (!size)
    {
        sn_error(SN_ERR_BAD_SIZE, NULL);
    }

    void* pr = malloc(size);

    if (!pr)
    {
        sn_error(SN_ERR_BAD_ALLOC, NULL);
    }

    linked_list_push(mem_list, pr, size, plat_getTid());

    return pr;
}



SN_PUB_API_OPEN void sn_free(void* const ptr)
{
    if (!ptr)
    {
        sn_error(SN_ERR_NULL_PTR);
    }

    linked_list_entry_c entry = linked_list_getByPtr(mem_list, ptr);
    if (!entry)
    {
        sn_error(SN_ERR_NO_ADDER_FOUND);
    }

    free(entry->data);

    linked_list_entry_destroy(entry);
}
