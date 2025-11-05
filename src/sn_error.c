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
#include "_pri_api.h"

static sn_error_codes_e error_code = SN_ERR_OK;

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

static const char* const human_readable_messages[191] = {
    [SN_ERR_OK] = "everything is AOK",
    [SN_ERR_NULL_PTR] = "Null-Pointer provided to function",
    [SN_ERR_NO_SIZE] = "no size Metadata Provided or available",
    [SN_ERR_BAD_SIZE] = "Invalid size provided",
    [SN_ERR_BAD_ALLOC] = "libc malloc Returned null",
    [SN_ERR_NO_ADDER_FOUND] = "no adder provided or available",
    [SN_ERR_NO_TID_FOUND] = "No tid found in system",
    [SN_ERR_BAD_BLOCK_ID] = "block id is not above 20",
    [SN_ERR_FILE_PRE_EXIST] = "file path provided already exists",
    [SN_ERR_FILE_IO] = "Libc Generic file IO error",
    [SN_ERR_FILE_NOT_EXIST] = "file Does not exist",
    [SN_ERR_ALLOC_LIMIT_HIT] = "User defined alloc limit has been hit",
    //[SN_WARN_DUB_FREE] = "Possible double free, but not found in registry",
    [SN_ERR_SYS_FAIL] = "generic system failure",
    [SN_ERR_CATASTROPHIC] = "Catastrophic system error",
    [SN_ERR_DEBUG] = "A debug error used in debug crashes",
    [SN_INFO_PLACEHOLDER] = "Undefined error Error code implementation coming soon"
};

static const char* const err_name_tap[191] = {
    [SN_ERR_OK] = "SN_ERR_OK",
    [SN_ERR_NULL_PTR] = "SN_ERR_NULL_PTR",
    [SN_ERR_NO_SIZE] = "SN_ERR_NO_SIZE",
    [SN_ERR_BAD_SIZE] = "SN_ERR_BAD_SIZE",
    [SN_ERR_BAD_ALLOC] = "SN_ERR_BAD_ALLOC",
    [SN_ERR_NO_ADDER_FOUND] = "SN_ERR_NO_ADDER_FOUND",
    [SN_ERR_NO_TID_FOUND] = "SN_ERR_NO_TID_FOUND",
    [SN_ERR_BAD_BLOCK_ID] = "SN_ERR_BAD_BLOCK_ID",
    [SN_ERR_FILE_PRE_EXIST] = "SN_ERR_FILE_PRE_EXIST",
    [SN_ERR_FILE_IO] = "SN_ERR_FILE_IO",
    [SN_ERR_FILE_NOT_EXIST] = "SN_ERR_FILE_NOT_EXIST",
    [SN_ERR_ALLOC_LIMIT_HIT] = "SN_ERR_ALLOC_LIMIT_HIT",
    //[SN_WARN_DUB_FREE] = "SN_WARN_DUB_FREE",
    [SN_ERR_SYS_FAIL] = "SN_ERR_SYS_FAIL",
    [SN_ERR_CATASTROPHIC] = "SN_ERR_CATASTROPHIC",
    [SN_ERR_DEBUG] = "SN_ERR_DEBUG",
    [SN_INFO_PLACEHOLDER] = "SN_INFO_PLACEHOLDER"
};

SN_PUB_API_OPEN const char* sn_get_error_msg(sn_error_codes_e err)
{
    const size_t tab_size = (sizeof(human_readable_messages) / sizeof(*human_readable_messages));

    // ReSharper disable once CppDFAConstantConditions
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

SN_PUB_API_OPEN
const char* sn_get_error_name(const sn_error_codes_e err)
{
    const size_t tab_size = (sizeof(err_name_tap) / sizeof(*err_name_tap));

    // ReSharper disable once CppDFAConstantConditions
    if ((err < SN_ERR_OK) || err >= tab_size)
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

SN_PUB_API_OPEN
void sn_set_last_error(const sn_error_codes_e err)
{
    plat_mutex_lock(alloc_mutex);
    error_code = err;
    plat_mutex_unlock(alloc_mutex);
}

