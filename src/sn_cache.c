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
// Created by tete on 10/11/2025.
//

#include "libsafetynet.h"
#include "_pri_api.h"

SN_PUB_API_OPEN SN_FLAG sn_request_to_fast_cache(const void* ptr)
{
    if (!ptr)
    {
        sn_error(SN_ERR_NULL_PTR, 0);
    }

    linked_list_entry_c entry = linked_list_getByPtr(mem_list, (void*)ptr);
    if (!entry)
    {
        sn_error(SN_ERR_NO_ADDER_FOUND, 0);
    }
    SN_FLAG ret = memman_tryCachePut(memory_manager, entry);
    return ret;
}

SN_PUB_API_OPEN void sn_lock_fast_cache()
{
    memory_manager->cache_lock = 1;
}

SN_PUB_API_OPEN void sn_unlock_fast_cache()
{
    memory_manager->cache_lock = 0;
}

SN_PUB_API_OPEN void sn_do_fast_caching(SN_FLAG val)
{
    memory_manager->use_cache = val;
}

SN_PUB_API_OPEN void sn_fast_cache_clear()
{
    memman_cacheClear(memory_manager);
}

