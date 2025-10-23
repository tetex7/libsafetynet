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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list_c.h"

#include "libsafetynet.h"
#include "_pri_api.h"


linked_list_c mem_list = NULL;
plat_mutex_c alloc_mutex = NULL;
alloc_manager_m memory_manager = NULL;
SN_FLAG doFree = 1;

static linked_list_entry_c freeOnListFree(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg)
{
    if (!doFree) return NULL;
    //if (!ctx) sn_debug_crash();
#ifdef SN_CONFIG_SANITIZE_MEMORY_ON_FREE
    memset(ctx->data, 0, ctx->size);
#endif
    free(ctx->data);
    return NULL;
}

void doexit()
{
    if (linked_list_getSize(mem_list))
    {
        linked_list_forEach(mem_list, &freeOnListFree, NULL);
    }
    plat_mutex_destroy(alloc_mutex);
    linked_list_destroy(mem_list);
    memman_destroy(memory_manager);
}

// Constructor: Called when the library is loaded
__attribute__((constructor))
void library_init()
{
    mem_list = linked_list_new();
    alloc_mutex = plat_mutex_new();
    memory_manager = memman_new(alloc_mutex);
}

// Destructor: Called when the library is unloaded
__attribute__((destructor))
void library_cleanup()
{
    // Perform any necessary cleanup, such as freeing global memory or other resources.
    // This function is called when the shared library is unloaded.
    doexit();
}
