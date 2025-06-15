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

#include "libsafetynet.h"
#include "sn_crash.h"
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "linked_list_c.h"

#include "_pri_api.h"

#define sn_crash_print(str, ...) fprintf(stderr, str, ##__VA_ARGS__)

static SN_VERY_OPTIMIZED int print_node(const linked_list_entry_c node)
{
    if (!node) return 0;

    int pcc = 0;
    pcc += sn_crash_print("previous: %p\n", node->previous);
    pcc += sn_crash_print("data: %p\n", node->data);
    pcc += sn_crash_print("size: %lu\n", node->size);
    pcc += sn_crash_print("tid: %lu\n", node->tid);
    pcc += sn_crash_print("isHead: %i", node->isHead);
    pcc += sn_crash_print("next: %p\n", node->next);
    return pcc;
}

SN_VERY_OPTIMIZED SN_NO_RET void __sn__pri__crash__(sn_error_codes_e err, uint32_t line, const char* file)
{
    sn_crash_print("Crash in libsafetynet/%s:%i :-(\n\n", file, line);
    sn_crash_print("ERROR: %i\n", err);
    sn_crash_print("ERROR_NAME: %s\n", sn_get_err_name(err));
    sn_crash_print("ERROR_MSG: %s\n", sn_get_error_msg(err));
    sn_crash_print("crash on tid %lu\n\n", pthread_self());
    if (err == SN_ERR_SYS_FAIL) goto EX1;

    sn_crash_print("Memory tracking list state:\n");
    sn_crash_print("\nlast_access_node:\n");
    print_node(mem_list->lastAccess);
    sn_crash_print("\n\n");

    EX1:
        exit(err);
}
