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

#pragma once
#ifndef LIB_SAFTY_NET_H
#define LIB_SAFTY_NET_H

#include <stddef.h>
#include <stdint.h>

#define SN_PUB_API_OPEN __attribute__((visibility("default")))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SN_ERR_OK = 0,          /**< No error */
    SN_ERR_NULL_PTR = 5,    /**< Null pointer passed to function */
    SN_ERR_NO_SIZE = 10,    /**< Memory block has no size information */
    SN_ERR_BAD_SIZE = 15,   /**< Invalid memory size */
    SN_ERR_BAD_ALLOC = 20,  /**< Memory allocation failed */
    SN_ERR_NO_ADDER_FOUND = 30, /**< No adder found in system */
    SN_WARN_DUB_FREE = 80   /**< Double free detected (warning) */
} sn_error_codes_e;

typedef size_t sn_mem_address_t;

/**
 * Allocates memory and tracks it for cleanup at program exit.
 * @param size The size of the memory block to allocate.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
SN_PUB_API_OPEN void* sn_malloc(size_t size);

/**
* Frees a tracked memory block.
* @param ptr Pointer to the memory block.
*/
SN_PUB_API_OPEN void  sn_free(void* const ptr);

/**
 * Registers a memory block for tracking.
 * @param ptr Pointer to the memory block.
 * @return The same pointer, or NULL on failure.
 */
SN_PUB_API_OPEN void* sn_register(void* const ptr);

/**
 * Queries the size of a tracked memory block.
 * @param ptr Pointer to the memory block.
 * @return The size of the memory block, or 0 on failure.
 */
SN_PUB_API_OPEN size_t sn_query_size(void* const ptr);

/**
* Queries the thread ID associated with a memory block.
* @param ptr Pointer to the memory block.
* @return The thread ID, or 0 on failure.
*/
SN_PUB_API_OPEN uint64_t sn_query_tid(void* const ptr);

/**
 * Registers a memory block with a specified size for tracking.
 * @param ptr Pointer to the memory block.
 * @param size Size of the memory block.
 * @return The same pointer, or NULL on failure.
 */
SN_PUB_API_OPEN void* sn_register_size(void* ptr, size_t size);

/**
* Retrieves the last error code.
* @return The last error code.
*/
SN_PUB_API_OPEN sn_error_codes_e sn_get_last_error();

/**
* Resets the last error code to SN_ERR_OK.
*/
SN_PUB_API_OPEN void sn_reset_last_error();

#ifdef __cplusplus
}
#endif

#endif