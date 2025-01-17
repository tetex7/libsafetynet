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
#ifndef LIB_SAFETY_NET_H
#define LIB_SAFETY_NET_H

#include <stddef.h>
#include <stdint.h>

#ifndef SN_PUB_API_OPEN
#   define SN_PUB_API_OPEN __attribute__((visibility("default")))
#endif
#ifndef SN_VERY_VOLATILE
#   define SN_VERY_VOLATILE __attribute__((optimize("O0")))
#endif
#ifndef SN_API_PREFIX
#   define SN_API_PREFIX(name) sn_##name
#endif
#ifndef SN_GET_ARR_SIZE
#   define SN_GET_ARR_SIZE(byte_size, type_size) ((size_t)(byte_size / type_size))
#endif
#ifndef SN_READONLY
#   define SN_READONLY const
#endif
#ifndef SN_BLOCK_NAME_MAX_LEN
#   define SN_BLOCK_NAME_MAX_LEN 100
#endif


#if (defined(SN_NO_STD_BOOL) || !__has_include(<stdbool.h>))
typedef uint8_t SN_BOOL;
typedef uint8_t SN_FLAG;
#define SN_TRUE 1
#define SN_FALSE 0
#define SN_FLAG_SET 1
#define SN_FLAG_UNSET 0
#else
#include <stdbool.h>
typedef bool SN_BOOL;
typedef bool SN_FLAG;
#define SN_TRUE 1
#define SN_FALSE 0
#define SN_FLAG_SET 1
#define SN_FLAG_UNSET 0
#endif

#ifdef __cplusplus
#   ifdef SN_ENABLE_CPP_NAMESPACE
namespace safetynet
{
#   endif
extern "C" {
#endif

typedef enum
{
    SN_ERR_OK = 0,                   /**< No error */
    SN_ERR_NULL_PTR = 5,             /**< Null pointer passed to function */
    SN_ERR_NO_SIZE = 10,             /**< Memory block has no size information */
    SN_ERR_BAD_SIZE = 15,            /**< Invalid memory size */
    SN_ERR_BAD_ALLOC = 20,           /**< Memory allocation failed */
    SN_ERR_NO_ADDER_FOUND = 30,      /**< No adder found in system */
    SN_ERR_NO_TID_FOUND = 40,        /**< No tid found in system */
    SN_WARN_DUB_FREE = 80            /**< Double free detected (warning) */
} sn_error_codes_e;

typedef size_t sn_mem_address_t;

/**
 * @brief Allocates memory and tracks it for cleanup at program exit.
 * @param size The size of the memory block to allocate.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
SN_PUB_API_OPEN void* SN_API_PREFIX(malloc)(size_t size);

/**
 * @brief Allocates memory and track it for an array of num objects of size and initializes it to all bits zero
 * @param num number of objects
 * @param size size of each object
 * @return Pointer to the allocated memory, or NULL on failure.
 */
SN_PUB_API_OPEN void* SN_API_PREFIX(calloc)(size_t num, size_t size);

/**
 * @brief Allocates memory and initializes it to a specified value.
 * @param size The size of the memory block to allocate.
 * @param initial_byte_value The value to initialize each byte of the allocated memory.
 * @return Pointer to the allocated memory, or NULL if allocation fails.
 */
SN_PUB_API_OPEN void* SN_API_PREFIX(malloc_pre_initialized)(size_t size, uint8_t initial_byte_value);

/**
* @brief Frees a tracked memory block.
* @param ptr Pointer to the memory block.
*/
SN_PUB_API_OPEN void SN_API_PREFIX(free)(void* const ptr);

/**
 * @brief Registers a memory block for tracking.
 * @param ptr Pointer to the memory block.
 * @return The same pointer, or NULL on failure.
 */
SN_PUB_API_OPEN void* SN_API_PREFIX(register)(void* const ptr);

/**
 * @brief Queries the size in Bytes of a tracked memory block.
 * @param ptr Pointer to the memory block.
 * @return The size of the memory block, or 0 on failure.
 */
SN_PUB_API_OPEN size_t SN_API_PREFIX(query_size)(void* const ptr);

/**
* @brief Queries the thread ID associated with a memory block.
* @param ptr Pointer to the memory block.
* @return The thread ID, or 0 on failure.
*/
SN_PUB_API_OPEN uint64_t SN_API_PREFIX(query_tid)(void* const ptr);

/**
 * @brief Registers a memory block with a specified size for tracking.
 * @param ptr Pointer to the memory block.
 * @param size Size of the memory block.
 * @return The same pointer, or NULL on failure.
 */
SN_PUB_API_OPEN void* SN_API_PREFIX(register_size)(void* ptr, size_t size);

/**
* @brief Retrieves the last error code.
* @return The last error code.
*/
SN_PUB_API_OPEN sn_error_codes_e SN_API_PREFIX(get_last_error)();

/**
* @brief Resets the last error code to SN_ERR_OK.
*/
SN_PUB_API_OPEN void SN_API_PREFIX(reset_last_error)();

/**
 * @brief Checks if a block is being tracked by the safety net system;
 * @param ptr Pointer to the block of memory
 * @return A flag to which it exists (a bool)
 */
SN_PUB_API_OPEN SN_FLAG SN_API_PREFIX(is_tracked_block)(const void* const ptr);

/**
 * @brief Provide you a human-readable error message
 * @param err The error code
 * @return A pointer to the string containing the error message (Do not manipulate the string Treat it as immutable)
 */
SN_PUB_API_OPEN const char* const SN_API_PREFIX(get_error_msg)(sn_error_codes_e err);

#ifdef __SN_WIP_CALLS__
typedef struct SN_API_PREFIX(mem_metadata_t)
{
    const void* const data;               // Pointer to data (generic data type)
    const size_t size;                    // size of the data
    const uint64_t tid;                   // The tid of the thread that allocated this chunk
    const SN_FLAG cached;                 // is mem block cached
} SN_API_PREFIX(mem_metadata_t);

/**
 * @brief This function provides a Read only view of the metadata structure for this memory block
 * @param ptr A pointer to a register block memory
 * @return returns null If nothing can be found otherwise it will return a pointer to the metadata
 */
SN_PUB_API_OPEN const sn_mem_metadata_t* SN_API_PREFIX(query_metadata)(void* ptr);

/**
 * @brief Adds the metadata associated with this block of memory to the fast cache
 * @param ptr A pointer to a Tracked block memory
 * @return Returns 1 if successfully added to fast cash 0 if it did not
 */
SN_PUB_API_OPEN const SN_FLAG SN_API_PREFIX(request_to_fast_cache)(const void* ptr);

/**
 * @brief Disables automatic fast caching of tracking metadata But the fast cash is still Queryed
 */
SN_PUB_API_OPEN void SN_API_PREFIX(lock_fast_cache)();

/**
 * @brief enables automatic fast caching of tracking metadata But the fast cash is still Queryed
 */
SN_PUB_API_OPEN void SN_API_PREFIX(unlock_fast_cache)();


/**
 * @brief Disables/enables the fast caching system entirely
 * @param val Is set to 1 enables it if set to 0 disables it
 */
SN_PUB_API_OPEN void SN_API_PREFIX(do_fast_caching)(SN_FLAG val);

/**
 * @brief Clears out the fast cache
 */
SN_PUB_API_OPEN void SN_API_PREFIX(fast_cache_clear)();

/**
 * @brief reAllocates memory and tracks it for cleanup at program exit.
 * @param ptr a pre Allocated block that's tracked
 * @param new_size The size of the memory block to reallocate.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
SN_PUB_API_OPEN void* SN_API_PREFIX(realloc)(void* ptr, size_t new_size);


/**
 * @brief Queries memory usage for a specific thread.
 * @param tid The thread ID.
 * @return Total memory used by the thread, or 0 if no memory is tracked for this thread.
 */
SN_PUB_API_OPEN size_t SN_API_PREFIX(query_thread_memory_usage)(uint64_t tid);

/**
 * @brief Queries total memory usage across all threads.
 * @return Total memory currently tracked.
 */
SN_PUB_API_OPEN size_t SN_API_PREFIX(query_total_memory_usage)();

#endif

#ifdef __cplusplus
}
#   ifdef SN_ENABLE_CPP_NAMESPACE
}
#   endif
#endif

#if defined(__cplusplus) && (__cplusplus >= 201703L) && defined(SN_ENABLE_NEW_CALLS)
#include <stdexcept>
namespace safetynet
{
    __inline void* operator new(size_t size)
    {
        void* p = sn_malloc(size);
        if (!p) throw std::bad_alloc();
        return p;
    }

    __inline void operator delete(void* p) noexcept
    {
        sn_free(p);
    }

    __inline void* operator new[](size_t size)
    {
        void* p = sn_malloc(size);
        if (!p) throw std::bad_alloc();
        return p;
    }

    __inline void operator delete[](void* p) noexcept
    {
        sn_free(p);
    }
}
#endif

#endif