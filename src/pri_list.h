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
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <libsafetynet.h>
#include <stddef.h>  // For size_t
#include <stdlib.h>  // For malloc, free
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

// Define the linked list node structure
typedef struct node_t {
    struct node_t* previous;        // Pointer to the next node
    void* data;                     // Pointer to data (generic data type)
    size_t size;                    // size of the data
    uint64_t tid;                   // The tid of the thread that allocated this chunk
    SN_FLAG cached;                 // is mem block cached
    uint16_t block_id;              // An optional block id
    uint8_t* extended_data;         // Extended optional data to be allocated separately
    uint8_t weight;                 // The amount of times this thing's been used since last the reorder
    struct node_t* next;            // Pointer to the next node
} node_t;



typedef struct node_pair_t
{
    const void* ptr_key;
    node_t* node;
} node_pair_t;

#define NODE_PAIR_INIT ((node_pair_t){NULL, NULL})

node_t* list_init();
node_t* list_query(node_t* head, const void* const data);
size_t  list_len(node_t* head);
node_t* list_index_to_last(node_t* head);
node_t* list_add(node_t* head, void* data);
void list_free_all(node_t *head);
void list_free(node_t* node);
void list_free_all_with_data(node_t* head);
node_t* list_query_by_id(node_t* head, uint16_t id);
SN_FLAG add_cache_node(node_t* head, const void* const ptr);

extern pthread_mutex_t list_mutex;
extern SN_FLAG list_caching;
extern SN_FLAG list_cache_lock;
extern node_pair_t caching_nodes[6];
extern const node_t* last_access_node;

#endif // LINKED_LIST_H