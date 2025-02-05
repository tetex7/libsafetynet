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
#include "crash.h"
#include "_pri_api.h"
#include "libsafetynet.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "pri_list.h"

sn_crash_trap_t crash_trap = NULL;


static int print_node(const node_t* node)
{
    if (!node) return 0;

    int pcc = 0;
    pcc += printf("previous: %p\n", node->previous);
    pcc += printf("data: %p\n", node->data);
    pcc += printf("size: %lu\n", node->size);
    pcc += printf("tid: %lu\n", node->tid);
    pcc += printf("cached: %i\n", node->cached);
    pcc += printf("block_id: %x\n", node->block_id);
    pcc += printf("extended_data: %p\n", node->extended_data);
    pcc += printf("weight: %i\n", node->weight);
    pcc += printf("next: %p\n", node->next);
    return pcc;
}

static const char* get_err_name(const sn_error_codes_e err)
{
    const size_t tab_size = SN_INFO_PLACEHOLDER;

    if ((err < 0) || err >= tab_size)
    {
        goto E1;
    }

    const char* str = err_name_tap[err];

    if (!str)
    {
        goto E1;
    }
    return str;

E1:
    return "SN_SERR_UNKNOWN";
}

SN_VERY_OPTIMIZED void __sn__pri__crash__(sn_error_codes_e err, uint32_t line, const char* file)
{
    if (crash_trap)
    {
        if (crash_trap(err, line, file)) return;
    }

    printf("Crash in libsafetynet/%s:%i :-(\n\n", file, line);
    printf("ERROR: %i\n", err);
    printf("ERROR_NAME: %s\n", get_err_name(err));
    printf("ERROR_MSG: %s\n", sn_get_error_msg(err));
    printf("crash on tid %lu\n\n", pthread_self());
    printf("Memory tracking list state:\n");
    printf("list fast caching: %i\n", list_caching);
    printf("list fast caching lock: %i\n", list_cache_lock);
    printf("\nlast_access_node:\n");
    print_node(last_access_node);
    printf("\n\n");

    for (size_t i = 0; i < 6; i++)
    {
        if (caching_nodes[i].node != NULL)
        {
            printf("\nfast_cache[%lu]:\n", i);
            print_node(caching_nodes[i].node);
            printf("\n");
        } else printf("fast_cache[%lu] = NULL\n", i);
    }
    exit(err);
}