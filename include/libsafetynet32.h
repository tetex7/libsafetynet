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
// Created by tete on 11/07/2025.
//

/*
 * This file contains a wrapper around the main API to create a Windows32 api like experience
 */

#pragma once

#ifndef LIBSAFETYNET32_H
#define LIBSAFETYNET32_H
#include <libsafetynet.h>

#ifndef SN_API32
#   define SN_API32 static inline
#endif
typedef sn_error_codes_e SN_ERR_CODE;
typedef void* SN_TRACKED_BLOCK_HANDLE;
typedef size_t SN_SIZE;
typedef sn_tid_t SN_TID;
typedef sn_block_id_t SN_BLOCK_ID;

SN_API32 SN_ERR_CODE SnAllocateTrackedMemoryBlock(SN_SIZE size, SN_TRACKED_BLOCK_HANDLE* out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_malloc(size);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnFreeTrackedMemoryBlock(SN_TRACKED_BLOCK_HANDLE block)
{
    sn_reset_last_error();
    sn_free(block);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnReallocateTrackedMemoryBlock(SN_SIZE new_size, SN_TRACKED_BLOCK_HANDLE* out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_realloc(*out_block, new_size);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnRegisterMemoryBlockForTracking(SN_TRACKED_BLOCK_HANDLE block, SN_SIZE size, SN_TRACKED_BLOCK_HANDLE* out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_register_size(block, size);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnSetTrackedMemoryBlockId(SN_TRACKED_BLOCK_HANDLE block, SN_BLOCK_ID id)
{
    sn_reset_last_error();
    sn_set_block_id(block, id);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnGetTrackedMemoryBlockById(SN_BLOCK_ID id, SN_TRACKED_BLOCK_HANDLE* out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_query_block_id(id);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnGetTrackedMemoryBlockId(SN_TRACKED_BLOCK_HANDLE block, SN_BLOCK_ID* out_id)
{
    sn_reset_last_error();
    if (!out_id) return SN_ERR_NULL_PTR;
    *out_id = sn_get_block_id(block);
    return sn_get_last_error();
}

SN_API32 SN_BOOL SnIsTrackedMemoryBlock(SN_TRACKED_BLOCK_HANDLE block)
{
    return sn_is_tracked_block(block);
}

#endif //LIBSAFETYNET32_H
