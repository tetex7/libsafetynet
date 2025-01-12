/*
 * Copyright (C) 2025  tete
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
#ifndef LIBSAFETYNET_EXTENSION_H
#define LIBSAFETYNET_EXTENSION_H

#ifndef SN_PUB_API_OPEN
#   define SN_PUB_API_OPEN __attribute__((visibility("default")))
#endif
#ifndef SN_VERY_VOLATILE
#   define SN_VERY_VOLATILE __attribute__((optimize("O0")))
#endif
#ifndef SN_API_PREFIX
#   define SN_API_PREFIX(name) sn_##name
#endif

#include <stddef.h>  // For size_t
#include <stdlib.h>  // For malloc, free
#include <stdint.h>


/* NOTE: List calls are not as well safeguarded as standard sn_* calls
 * I do not know if this API will ever become fully functional
 */

// Define the linked list node structure
typedef struct sn_node_t {
    struct sn_node_t* previous;  // Pointer to the next node
    void* data;                  // Pointer to data (generic data type)
    size_t size;                 // size of the data
    uint64_t tid;                // The tid of the thread that allocated this chunk
    void* extended_data;         // Extended optional data to be allocated separately
    uint8_t tto_order;           // Once reaches the integer limit wel'll then rollback to zero and reorder the list
    uint8_t weight;              // The amount of times this thing's been used since last the reorder
    struct sn_node_t* next;      // Pointer to the next node
} sn_node_t;

typedef struct sn_extension_t
{
    sn_node_t* const mem_list_head;
    sn_node_t* (*list_init)();
    sn_node_t* (*list_query)(sn_node_t* head, const void* const data);
    size_t  (*list_len)(sn_node_t* head);
    sn_node_t* (*list_index_to_last)(sn_node_t* head);
    sn_node_t* (*list_add)(sn_node_t* head, void* data);
    void (*list_free_all)(sn_node_t *head);
    void (*list_free)(sn_node_t* node);
    void (*list_free_all_with_data)(sn_node_t* head);
} sn_extension_t;

SN_PUB_API_OPEN sn_extension_t SN_API_PREFIX(get_extension_data)();

#endif //LIBSAFETYNET_EXTENSION_H
