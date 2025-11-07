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

#include "platform_independent/plat_threading.h"
#include "allocation_manager/alloc_manager_c.h"

#include "_pri_api.h"

#define sn_crash_print(str, ...) fprintf(stderr, str, ##__VA_ARGS__)

static int print_node(const linked_list_entry_c node)
{
    int pcc = 0;
    if (!node)
    {
        pcc += sn_crash_print("NODE IS NULL\n");
        return pcc;
    }
    pcc += sn_crash_print("node@%p(head=%i)\n", node, node->isHead);
    pcc += sn_crash_print("previous: %p\n", node->previous);
    pcc += sn_crash_print("data: %p\n", node->data);
#ifdef SN_ON_UNIX
    pcc += sn_crash_print("size: %lu\n", node->size);
    pcc += sn_crash_print("tid: %lu\n", node->tid);
#elif defined(SN_ON_WIN32)
    pcc += sn_crash_print("size: %llu\n", node->size);
    pcc += sn_crash_print("tid: %llu\n", node->tid);
#endif
    pcc += sn_crash_print("isHead: %i\n", node->isHead);
    pcc += sn_crash_print("next: %p\n", node->next);
    pcc += sn_crash_print("weight : %x\n", node->_weight);
    return pcc;
}

static linked_list_entry_c list_nodeas(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg)
{
    if (!ctx)
    {
#ifdef SN_ON_UNIX
        sn_crash_print("node: %lu is NULL@%p\n", index, ctx);
#elif defined(SN_ON_WIN32)
        sn_crash_print("node: %lluis NULL@%p\n", index, ctx);
#endif
        return LIST_FOR_EACH_LOOP_BRAKE;
    }
#ifdef SN_ON_UNIX
    sn_crash_print("node: %lu\n", index);
#elif defined(SN_ON_WIN32)
    sn_crash_print("node: %llu\n", index);
#endif
    print_node(ctx);
    sn_crash_print("\n");
    return NULL;
}


#ifdef SN_CONFIG_ENABLE_STACK_TRACE

static void error_callback(void *data, const char *msg, int errnum)
{
    sn_crash_print("libbacktrace error: %s (err %d)\n", msg, errnum);
}

static int full_callback(void *data, uintptr_t pc,
                         const char *filename, int lineno,
                         const char *function)
{
    sn_crash_print("  %s:%d: %s (pc = %p)\n",
           filename ? filename : "??",
           lineno,
           function ? function : "??",
           (void *)pc);
    return 0; // 0 = continue
}

static void print_stack_trace()
{
    struct backtrace_state *state =
        backtrace_create_state(NULL, 1, error_callback, NULL);

    //printf("Stack trace:\n");
    backtrace_full(state, 0, full_callback, error_callback, NULL);
}
#endif


SN_NO_RETURN void __sn__pri__crash__(const sn_error_codes_e err, const uint32_t line, const char* file, const char* func_call_name)
{
    sn_crash_print("Crash in libsafetynet/%s:%i(%s) :-(\n\n", file, line, func_call_name);
    sn_crash_print("ERROR: %i\n", err);
    sn_crash_print("ERROR_NAME: %s\n", sn_get_error_name(err));
    sn_crash_print("ERROR_MSG: %s\n", sn_get_error_msg(err));
#ifdef SN_ON_UNIX
    sn_crash_print("crash on tid 0x%lx\n\n", plat_getTid());
#elif defined(SN_ON_WIN32)
    sn_crash_print("crash on tid 0x%llx\n\n", plat_getTid());
#endif
#ifdef SN_CONFIG_ENABLE_STACK_TRACE
    sn_crash_print("\nStock trace:\n");
    print_stack_trace();
    sn_crash_print("\n");
#endif

    if (err == SN_ERR_CATASTROPHIC) abort();
    if (err == SN_ERR_SYS_FAIL) goto EX1;

    sn_crash_print("Memory tracking list state:\n");
    sn_crash_print("\nlast_access_node:\n");
    print_node(mem_list->lastAccess);

    sn_crash_print("\nNode Fast cache\n");
    sn_crash_print("Cache Enabled: %d\n", memory_manager->use_cache);
    sn_crash_print("Cache Locked: %d\n", memory_manager->cache_lock);
    sn_crash_print("Number of Baked-in Cache Slots: %d\n", MEMMAN_MAX_CACHE_SLOTS);
    sn_crash_print("Available Cache Slots: %d\n", memory_manager->available_cache_slots);
    if (memory_manager->available_cache_slots != MEMMAN_MAX_CACHE_SLOTS)
    {
        for (size_t i = 0; i < MEMMAN_MAX_CACHE_SLOTS; i++)
        {
            if (memory_manager->cache_list[i].key != NULL)
            {
                print_node(memory_manager->cache_list[i].value);
            }
        }
    }
    else sn_crash_print("Nothing in the Cache\n");

#ifdef SN_CONFIG_ENABLE_DUMP_LIST_CRASH
    if (linked_list_getSize(mem_list) != 0)
    {
        sn_crash_print("\n\n");
        sn_crash_print("nodes:\n");
        linked_list_forEach(mem_list, &list_nodeas, NULL);
    }
#endif

EX1:
    exit(err);
}
