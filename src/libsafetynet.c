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

#include <stdio.h>
#include <string.h>

#include "pri_list.h"
#include "_pri_api.h"



extern node_t* mem_list;
pthread_mutex_t last_error_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t alloc_mutex = PTHREAD_MUTEX_INITIALIZER;
static size_t bytes_alloc = 0;
static size_t alloc_limit = 0;

static SN_BOOL can_aloc(size_t size)
{
    if (!alloc_limit)
    {
        return SN_TRUE;
    }
    if ((bytes_alloc == alloc_limit) || ((size + bytes_alloc) < alloc_limit))
    {
        return SN_FALSE;
    }
    return SN_TRUE;
}


/**
 *
 * @param size Size in bytes to be allocated
 * @return a block Allocated or null on failure use sn_get_last_error() for more info
 */
SN_PUB_API_OPEN
void* sn_malloc(const size_t size)
{
    if (!size)
    {
        sn_set_last_error(SN_ERR_BAD_SIZE);
        return NULL;
    }

    if (!can_aloc(size))
    {
        sn_set_last_error(SN_ERR_ALLOC_LIMIT_HIT);
        return NULL;
    }

    void* ret = malloc(size);

    if (!ret)
    {
        sn_set_last_error(SN_ERR_BAD_ALLOC);
        return NULL;
    }

    node_t* n = list_add(mem_list, ret);
    pthread_mutex_lock(&alloc_mutex);
    n->size = size;
    bytes_alloc += size;
    pthread_mutex_unlock(&alloc_mutex);
    return ret;
}

SN_PUB_API_OPEN
void sn_free(void* const ptr)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return;
    }

    node_t* node = list_query(mem_list, ptr);
    if (node)
    {
        free(node->data);
        bytes_alloc -= node->size;
        list_free(node);
        return;
    }
    sn_set_last_error(SN_WARN_DUB_FREE);
}

SN_PUB_API_OPEN
void* sn_register(void* const ptr)
{
    sn_set_last_error(SN_ERR_NO_SIZE);
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return NULL;
    }
    node_t* q = list_query(mem_list, ptr);
    if (!q)
    {
        list_add(mem_list, ptr);
        return ptr;
    }
    return ptr;
}

SN_PUB_API_OPEN
size_t sn_query_size(void* const ptr)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return 0;
    }
    node_t* q = list_query(mem_list, ptr);
    if (q)
    {
        if(!q->size)
            sn_set_last_error(SN_ERR_NO_SIZE);
        return q->size;
    }
    sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
    return 0;
}

SN_PUB_API_OPEN
uint64_t sn_query_tid(void *const ptr)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return 0;
    }

    node_t* q = list_query(mem_list, ptr);
    if (q)
    {
        return q->tid;
    }
    sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
    return 0;
}

SN_PUB_API_OPEN
void* sn_register_size(void *ptr, size_t size)
{
    if (!size)
    {
        sn_set_last_error(SN_ERR_BAD_SIZE);
        return NULL;
    }

    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return NULL;
    }
    node_t* q = list_query(mem_list, ptr);
    if (!q)
    {
        node_t* temp = list_add(mem_list, ptr);
        temp->size = size;
        return ptr;
    }
    sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
    return NULL;
}

SN_PUB_API_OPEN
sn_error_codes_e sn_get_last_error()
{
    return error_code;
}

SN_PUB_API_OPEN
void sn_reset_last_error()
{
    pthread_mutex_lock(&last_error_mutex);
    error_code = SN_ERR_OK;
    pthread_mutex_unlock(&last_error_mutex);
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
    [SN_ERR_FTRUNCATE_CALL_FAILED] = "posix call to MSYNC failed",
    [SN_ERR_FILE_NOT_EXIST] = "file Does not exist",
    [SN_WARN_DUB_FREE] = "Possible double free, but not found in registry",
    [SN_INFO_PLACEHOLDER] = "Undefined error Error code implementation coming soon"
};


/**
 * Provide you a human-readable error message
 * @param err The error code
 * @return A pointer to the string containing the error message (Do not manipulate the string Treat it as immutable)
 */
SN_PUB_API_OPEN const char* const SN_API_PREFIX(get_error_msg)(sn_error_codes_e err)
{
    const char* str = human_readable_messages[err];
    const size_t tab_size = (sizeof(human_readable_messages) / sizeof(*human_readable_messages));

    if ((err < 0) || err >= tab_size)
    {
        goto E1;
    }

    if (!str)
    {
        goto E1;
    }
    return str;

E1:
    return "Unknown error";
}

SN_PUB_API_OPEN const sn_mem_metadata_t* SN_API_PREFIX(query_metadata)(void *ptr)
{

    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return NULL;
    }
    node_t* q = list_query(mem_list, ptr);
    if (q)
    {
        return (sn_mem_metadata_t*)&q->data;
    }
    sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
    return NULL;
}

/**
 * @brief Checks if a block is being tracked by the safety net system;
 * @param ptr Pointer to the block of memory
 * @return A flag to which it exists (a bool)
 */
SN_PUB_API_OPEN SN_FLAG SN_API_PREFIX(is_tracked_block)(const void* const ptr)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return SN_FLAG_UNSET;
    }
    const node_t* q = list_query(mem_list, ptr);
    if (q)
    {
        return SN_FLAG_SET;
    }
    return SN_FLAG_UNSET;
}


SN_PUB_API_OPEN const SN_FLAG SN_API_PREFIX(request_to_fast_cache)(const void* ptr)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return SN_FLAG_UNSET;
    }

    if (!list_query(mem_list, ptr))
    {
        sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
    }

    return add_cache_node(mem_list, ptr);
}

SN_PUB_API_OPEN void SN_API_PREFIX(do_fast_caching)(SN_FLAG val)
{
    pthread_mutex_lock(&list_mutex);
    list_caching = val;
    pthread_mutex_unlock(&list_mutex);
}

SN_PUB_API_OPEN void* SN_API_PREFIX(realloc)(void* ptr, size_t new_size)
{
    if (!ptr)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return NULL;
    }

    node_t* n = list_query(mem_list, ptr);
    if (!n)
    {
        sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
        return NULL;
    }

    if (!new_size)
    {
        sn_set_last_error(SN_ERR_BAD_SIZE);
        return NULL;
    }

    void* ret = realloc(ptr, new_size);

    if (!ret)
    {
        sn_set_last_error(SN_ERR_BAD_ALLOC);
        return NULL;
    }


    n->data = ret;
    n->size = new_size;
    return ret;
}

SN_PUB_API_OPEN void* SN_API_PREFIX(calloc)(size_t num, size_t size)
{
    if (!size)
    {
        sn_set_last_error(SN_ERR_BAD_SIZE);
        return NULL;
    }

    void* ret = calloc(num, size);

    if (!ret)
    {
        sn_set_last_error(SN_ERR_BAD_ALLOC);
        return NULL;
    }

    node_t* n = list_add(mem_list, ret);
    n->size = size;
    return ret;
}

SN_PUB_API_OPEN void* SN_API_PREFIX(malloc_pre_initialized)(size_t size, uint8_t initial_byte_value)
{
    uint8_t* buff = sn_malloc(size);

    if (!buff) return NULL;

    memset(buff, initial_byte_value, size);

    return buff;
}

SN_PUB_API_OPEN void SN_API_PREFIX(lock_fast_cache)()
{
    pthread_mutex_lock(&list_mutex);
    list_cache_lock = SN_FLAG_SET;
    pthread_mutex_unlock(&list_mutex);
}

SN_PUB_API_OPEN void SN_API_PREFIX(unlock_fast_cache)()
{
    pthread_mutex_lock(&list_mutex);
    list_cache_lock = SN_FLAG_UNSET;
    pthread_mutex_unlock(&list_mutex);
}

SN_PUB_API_OPEN size_t SN_API_PREFIX(query_thread_memory_usage)(uint64_t tid)
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    size_t tid_memory_usage = 0;

    for (node_t* current_node = mem_list; current_node != NULL; current_node=current_node->next)
    {
        if (current_node->tid == tid)
        {
            tid_memory_usage+=current_node->size;
        }
    }
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    return tid_memory_usage;
}

SN_PUB_API_OPEN size_t SN_API_PREFIX(query_total_memory_usage)()
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    size_t total_memory_usage = 0;

    for (node_t* current_node = mem_list; current_node != NULL; current_node=current_node->next)
    {
        total_memory_usage+=current_node->size;
    }
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    return total_memory_usage;
}


SN_PUB_API_OPEN void sn_set_block_id(void* block, uint16_t id)
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    if (!block)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
        return;
    }

    if (id < 20)
    {
        sn_set_last_error(SN_ERR_BAD_BLOCK_ID);
        pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
        return;
    }

    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    node_t* n = list_query(mem_list, block);
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    if (!n)
    {
        sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
        pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
        return;
    }
    n->block_id = id;
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
}


SN_PUB_API_OPEN uint16_t SN_API_PREFIX(get_block_id)(void* block)
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex

    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    node_t* n = list_query(mem_list, block);
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    if (!n)
    {
        sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
        pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
        return 0;
    }
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    return n->block_id;
}

SN_PUB_API_OPEN void* SN_API_PREFIX(query_block_id)(uint16_t id)
{

    if (id < 20)
    {
        sn_set_last_error(SN_ERR_BAD_BLOCK_ID);
        pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
        return NULL;
    }

    node_t* n = list_query_by_id(mem_list, id);
    if (!n)
    {
        sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
        pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
        return NULL;
    }
    return n->data;
}

SN_PUB_API_OPEN void SN_API_PREFIX(do_auto_free_at_exit)(SN_FLAG val)
{
    pthread_mutex_lock(&last_error_mutex);
    do_free_exit = val;
    pthread_mutex_unlock(&last_error_mutex);
}

SN_PUB_API_OPEN uint64_t SN_API_PREFIX(calculate_checksum)(void* block)
{
    if (!block)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return 0;
    }

    node_t* n = list_query(mem_list, block);
    if (!n)
    {
        sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
        return 0;
    }

    if (!n->size)
    {
        sn_set_last_error(SN_ERR_NO_SIZE);
        return 0;
    }

    const uint8_t* const data = block;

    if (n->size == 1)
    {
        return *data ^ 0xFF;
    }

    uint64_t checksum = 0;
    const uint8_t first_value = *data;
    const uint8_t last_value = data[n->size - 1];
    const uint8_t mid_value = data[n->size/2];
    const uint8_t mixer = ~(first_value ^ last_value) ^ mid_value;
    for (size_t i = 0; i < n->size; i++)
    {
        checksum = ((((data[i] ^ mixer) + mid_value) * last_value) + (*(uint8_t*)&i)) ^ checksum;
    }
    return (checksum);
}

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/stat.h>
/**
 * @brief Dumps the contents of a track block memory to a file
 * @param file Path to a nonexistent file
 * @param block A pointer to a tracked block of memory
 * @return If 0 failure, if 1 successful
 */
SN_PUB_API_OPEN SN_FLAG SN_API_PREFIX(dump_to_file)(const char* file, void* block)
{
    if (!block)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return 0;
    }

    if (!file)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return 0;
    }

    node_t* n = list_query(mem_list, block);
    if (!n)
    {
        sn_set_last_error(SN_ERR_NO_ADDER_FOUND);
        return 0;
    }

    if (!n->size)
    {
        sn_set_last_error(SN_ERR_NO_SIZE);
        return 0;
    }

    if (access(file, F_OK) == 0)
    {
        sn_set_last_error(SN_ERR_DUMP_FILE_PREEXIST);
        return 0;
    }

    const int dump_file = open(file, O_CREAT | O_RDWR, 0644);

    if (ftruncate(dump_file, n->size) < 0)
    {
        sn_set_last_error(SN_ERR_FTRUNCATE_CALL_FAILED);
        close(dump_file);
        return 0;
    }

    void* file_buff = mmap(NULL, n->size, PROT_READ | PROT_WRITE, MAP_SHARED, dump_file, 0);

    if (file_buff == MAP_FAILED)
    {
        sn_set_last_error(SN_ERR_MMAP_CALL_FAILED);
        close(dump_file);
        return 0;
    }

    memcpy(file_buff, block, n->size);

    if (msync(file_buff, n->size, MS_SYNC) < 0)
    {
        sn_set_last_error(SN_ERR_MSYNC_CALL_FAILED);
    }

    if (munmap(file_buff, n->size) < 0) {
        sn_set_last_error(SN_ERR_MUNMAP_CALL_FAILED);
    }
    close(dump_file);

    return 1;
}

/**
 * @brief It copies a files data to block memory of the same size
 * @param file Path to a preexisting file (This file will be treated as read only)
 * @return A pointer to a Pre-allocated tracked block of memory
 */
SN_PUB_API_OPEN void* SN_API_PREFIX(mount_file_to_ram)(const char* file)
{
    if (!file)
    {
        sn_set_last_error(SN_ERR_NULL_PTR);
        return NULL;
    }

    struct stat fileStat;

    if (stat(file, &fileStat) != 0)
    {
        sn_set_last_error(SN_ERR_FILE_NOT_EXIST);
        return NULL;
    }

    const int file_obj = open(file, O_CREAT | O_RDONLY, 0644);
    const size_t file_size = fileStat.st_size;

    void* buff = sn_malloc(file_size);
    if (!buff)
    {
        close(file_obj);
        return NULL;
    }

    void* file_buff = mmap(NULL, file_size, PROT_READ, MAP_SHARED, file_obj, 0);

    if (file_buff == MAP_FAILED)
    {
        sn_set_last_error(SN_ERR_MMAP_CALL_FAILED);
        close(file_obj);
        return 0;
    }

    memcpy(buff, file_buff, file_size);

    if (msync(file_buff, file_size, MS_SYNC) < 0)
    {
        sn_set_last_error(SN_ERR_MSYNC_CALL_FAILED);
    }

    if (munmap(file_buff, file_size) < 0) {
        sn_set_last_error(SN_ERR_MUNMAP_CALL_FAILED);
    }
    close(file_obj);

    return buff;
}