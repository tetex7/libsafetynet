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

/**
 * @file libsafetynet32.h
 * @brief This file contains a wrapper around the main API to create a Windows32 api like experience
 */

#pragma once

#ifndef LIBSAFETYNET32_H
#define LIBSAFETYNET32_H
#include <libsafetynet.h>

#ifdef __cplusplus
#define SN32_CPP_COMPAT_START extern "C" {
#define SN32_CPP_COMPAT_END }
#else
#define SN32_CPP_COMPAT_START
#define SN32_CPP_COMPAT_END
#endif

SN32_CPP_COMPAT_START

#ifndef SN_API32
#   ifdef __cplusplus
#       define SN_API32 inline
#   else
#       define SN_API32 static inline
#   endif
#endif
typedef sn_error_codes_e SN_ERR_CODE;
typedef void* SN_TRACKED_BLOCK_HANDLE;
typedef size_t SN_SIZE;
typedef sn_tid_t SN_TID;
typedef sn_block_id_t SN_BLOCK_ID;
typedef size_t SN_BLOCK_ELEMENT_SIZE;
typedef const char* SN_CHAR_STRING;

typedef uint8_t SN_BYTE;
typedef uint16_t SN_WORD;
typedef uint32_t SN_DWORD;
typedef uint64_t SN_QWORD;

#define SN32_OUT
#define SN32_IN
#define SN32_OPTIONAL

SN_API32 SN_ERR_CODE SnAllocateTrackedMemoryBlock(SN_SIZE size, SN_TRACKED_BLOCK_HANDLE*  out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_malloc(size);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnReallocateTrackedMemoryBlock(SN_SIZE new_size, SN_TRACKED_BLOCK_HANDLE* out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_realloc(*out_block, new_size);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnAllocateTrackedMemoryBlockByArrayElementSize(
    SN_SIZE arr_size,
    SN_BLOCK_ELEMENT_SIZE element_size,
    SN_TRACKED_BLOCK_HANDLE* out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_calloc(arr_size, element_size);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnAllocatePreInitializedTrackedMemoryBlock(
    SN_SIZE size,
    SN_BYTE initial_byte_value,
    SN_TRACKED_BLOCK_HANDLE* out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_malloc_pre_initialized(size, initial_byte_value);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnFreeTrackedMemoryBlock(SN_TRACKED_BLOCK_HANDLE block)
{
    sn_reset_last_error();
    sn_free(block);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnRegisterMemoryBlockForTracking(SN_TRACKED_BLOCK_HANDLE block, SN_SIZE size)
{
    sn_reset_last_error();
    sn_register_size(block, size);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnSetTrackedMemoryBlockId(SN_TRACKED_BLOCK_HANDLE block, SN_BLOCK_ID id)
{
    sn_reset_last_error();
    sn_set_block_id(block, id);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnQueryTrackedMemoryBlockById(SN_BLOCK_ID id, SN_TRACKED_BLOCK_HANDLE* out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_query_block_id(id);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnQueryTrackedMemoryBlockId(SN_TRACKED_BLOCK_HANDLE block, SN_BLOCK_ID* out_id)
{
    sn_reset_last_error();
    if (!out_id) return SN_ERR_NULL_PTR;
    *out_id = sn_get_block_id(block);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnQueryTrackedMemoryBlockAllocatingTid(SN_TRACKED_BLOCK_HANDLE block, SN_TID* out_tid)
{
    sn_reset_last_error();
    if (!out_tid) return SN_ERR_NULL_PTR;
    *out_tid = sn_query_tid(block);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnQueryTrackedMemoryBlockSize(SN_TRACKED_BLOCK_HANDLE block, SN_SIZE* out_size)
{
    sn_reset_last_error();
    if (!out_size) return SN_ERR_NULL_PTR;
    *out_size = sn_query_size(block);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnQueryArrayCountFromTrackedBlock(
    SN_TRACKED_BLOCK_HANDLE block,
    SN_BLOCK_ELEMENT_SIZE element_size,
    SN_SIZE* out_size)
{
    sn_reset_last_error();
    if (!out_size) return SN_ERR_NULL_PTR;
    *out_size = sn_query_size_in_block_size(block, element_size);
    return sn_get_last_error();
}

SN_API32 SN_BOOL SnIsTrackedMemoryBlock(SN_TRACKED_BLOCK_HANDLE block)
{
    return sn_is_tracked_block(block);
}

SN_API32 SN_CHAR_STRING SnGetErrorStringName(SN_ERR_CODE err)
{
    return sn_get_error_name(err);
}

SN_API32 SN_CHAR_STRING SnGetErrorDescription(SN_ERR_CODE err)
{
    return sn_get_error_msg(err);
}

SN_API32 SN_ERR_CODE SnSaveTrackedMemoryBlockToBinFile(SN_CHAR_STRING file, SN_TRACKED_BLOCK_HANDLE block)
{
    sn_reset_last_error();
    sn_dump_to_file(file, block);
    return sn_get_last_error();
}

SN_API32 SN_ERR_CODE SnMountBinFileAsTrackedMemoryBlock(SN_CHAR_STRING file, SN_TRACKED_BLOCK_HANDLE* out_block)
{
    sn_reset_last_error();
    if (!out_block) return SN_ERR_NULL_PTR;
    *out_block = sn_mount_file_to_ram(file);
    return sn_get_last_error();
}

SN32_CPP_COMPAT_END

#undef SN32_CPP_COMPAT_END
#undef SN32_CPP_COMPAT_START

#endif //LIBSAFETYNET32_H
