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

sn_error_codes_e error_code = SN_ERR_OK;

SN_PUB_API_OPEN
sn_error_codes_e sn_get_last_error()
{
    return error_code;
}

SN_PUB_API_OPEN
void sn_reset_last_error()
{
    error_code = SN_ERR_OK;
}

static const char* const human_readable_messages[] = {
    [SN_ERR_OK] = "everything is AOK",
    [SN_ERR_NULL_PTR] = "Null-Pointer provided to function",
    [SN_ERR_NO_SIZE] = "no size Metadata Provided or available",
    [SN_ERR_BAD_SIZE] = "Invalid size provided",
    [SN_ERR_BAD_ALLOC] = "libc malloc Returned null",
    [SN_ERR_NO_ADDER_FOUND] = "no adder provided or available",
    [SN_ERR_NO_TID_FOUND] = "No tid found in system",
    [SN_ERR_BAD_BLOCK_ID] = "block id is not above 20",
    [SN_ERR_DUMP_FILE_PREEXIST] = "Dump file path provided already exists",
    [SN_ERR_MSYNC_CALL_FAILED] = "posix call to MSYNC failed",
    [SN_ERR_MMAP_CALL_FAILED] = "posix call to mmap failed",
    [SN_ERR_MUNMAP_CALL_FAILED] = "posix call to munmap failed",
    [SN_ERR_FTRUNCATE_CALL_FAILED] = "posix call to FTRUNCATE failed",
    [SN_ERR_FILE_NOT_EXIST] = "file Does not exist",
    [SN_ERR_ALLOC_LIMIT_HIT] = "User defined alloc limit has been hit",
    [SN_WARN_DUB_FREE] = "Possible double free, but not found in registry",
    [SN_ERR_SYS_FAIL] = "generic system failure",
    [SN_INFO_PLACEHOLDER] = "Undefined error Error code implementation coming soon"
};

static const char* const err_name_tap[] = {
    [SN_ERR_OK] = "SN_ERR_OK",
    [SN_ERR_NULL_PTR] = "SN_ERR_NULL_PTR",
    [SN_ERR_NO_SIZE] = "SN_ERR_NO_SIZE",
    [SN_ERR_BAD_SIZE] = "SN_ERR_BAD_SIZE",
    [SN_ERR_BAD_ALLOC] = "SN_ERR_BAD_ALLOC",
    [SN_ERR_NO_ADDER_FOUND] = "SN_ERR_NO_ADDER_FOUND",
    [SN_ERR_NO_TID_FOUND] = "SN_ERR_NO_TID_FOUND",
    [SN_ERR_BAD_BLOCK_ID] = "SN_ERR_BAD_BLOCK_ID",
    [SN_ERR_DUMP_FILE_PREEXIST] = "SN_ERR_DUMP_FILE_PREEXIST",
    [SN_ERR_MSYNC_CALL_FAILED] = "SN_ERR_MSYNC_CALL_FAILED",
    [SN_ERR_MMAP_CALL_FAILED] = "SN_ERR_MMAP_CALL_FAILED",
    [SN_ERR_MUNMAP_CALL_FAILED] = "SN_ERR_MUNMAP_CALL_FAILED",
    [SN_ERR_FTRUNCATE_CALL_FAILED] = "SN_ERR_FTRUNCATE_CALL_FAILED",
    [SN_ERR_FILE_NOT_EXIST] = "SN_ERR_FILE_NOT_EXIST",
    [SN_ERR_ALLOC_LIMIT_HIT] = "SN_ERR_ALLOC_LIMIT_HIT",
    [SN_WARN_DUB_FREE] = "SN_WARN_DUB_FREE",
    [SN_ERR_SYS_FAIL] = "SN_ERR_SYS_FAIL",
    [SN_INFO_PLACEHOLDER] = "SN_INFO_PLACEHOLDER"
};


/**
 * Provide you a human-readable error message
 * @param err The error code
 * @return A pointer to the string containing the error message (Do not manipulate the string Treat it as immutable)
 */
SN_PUB_API_OPEN const char* const sn_get_error_msg(sn_error_codes_e err)
{
    const size_t tab_size = (sizeof(human_readable_messages) / sizeof(*human_readable_messages));

    if ((err < SN_ERR_OK) || err >= tab_size)
    {
        goto E1;
    }

    const char* str = human_readable_messages[err];

    if (!str)
    {
        goto E1;
    }
    return str;

E1:
    return "Unknown error";
}

const char* sn_get_err_name(const sn_error_codes_e err)
{
    const size_t tab_size = SN_INFO_PLACEHOLDER;

    if ((err < SN_ERR_OK) || (err >= tab_size))
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
        return "SN_SOFT_FAKE_ERR_UNKNOWN";
}

