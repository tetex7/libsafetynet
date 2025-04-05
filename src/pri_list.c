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
#include "pri_list.h"
#include "crash.h"

#include <extended_data.h>
#include <pthread.h>
#include <string.h>
#include <_pri_api.h>

pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
SN_FLAG list_cache_lock = SN_FLAG_UNSET;
static uint8_t count_cache = 0;
SN_FLAG list_caching = SN_FLAG_SET;
uint8_t tto_order = 0;              // Once reaches the integer limit wel'll then rollback to zero and reorder the list
const node_t* last_access_node = NULL;

node_pair_t caching_nodes[6] = {
    NODE_PAIR_INIT,
    NODE_PAIR_INIT,
    NODE_PAIR_INIT,
    NODE_PAIR_INIT,
    NODE_PAIR_INIT,
    NODE_PAIR_INIT,
};


void cache_clear()
{
    pthread_mutex_lock(&list_mutex);
    for (size_t i = 0; i < 6; i++)
    {
        if (caching_nodes[i].ptr_key != NULL)
        {
            caching_nodes[i].node->cached = SN_FLAG_UNSET;
            caching_nodes[i].node = NULL;
            caching_nodes[i].ptr_key = NULL;
        }
    }
    count_cache = 0;
    pthread_mutex_unlock(&list_mutex);
}

SN_FLAG add_cache_node(node_t* head, const void* const ptr)
{
    if (!list_caching) return SN_FLAG_UNSET;
    pthread_mutex_lock(&list_mutex);
    if (caching_nodes[count_cache].node == NULL)
    {
        pthread_mutex_unlock(&list_mutex);
        node_t* qp = list_query(head, ptr);
        pthread_mutex_lock(&list_mutex);
        if (qp != NULL)
        {
            caching_nodes[count_cache].ptr_key = ptr;
            caching_nodes[count_cache].node = qp;
            caching_nodes[count_cache].node->weight = 0;
            caching_nodes[count_cache].node->cached = SN_FLAG_SET;
            (count_cache == 6) ? count_cache = 0 : count_cache++;
            pthread_mutex_unlock(&list_mutex);
            return SN_FLAG_SET;
        }
    }
    pthread_mutex_unlock(&list_mutex);
    return SN_FLAG_UNSET;
}

static void remove_cached_ptr(const void* ptr)
{
    for (size_t i = 0; i < 6; i++)
    {
        if (caching_nodes[i].ptr_key != ptr)
        {
            caching_nodes[i].node->cached = SN_FLAG_UNSET;
            caching_nodes[i].node = NULL;
            caching_nodes[i].ptr_key = NULL;
        }
    }
}

static node_t* get_caching_node(const void* const ptr)
{
    if (!list_caching) return NULL;
    for (size_t i = 0; i < 6; i++)
    {
        if (caching_nodes[i].ptr_key == ptr)
        {
            return caching_nodes[i].node;
        }
    }
    return NULL;
}


static node_t* get_caching_node_by_id(uint16_t id)
{
    if (!list_caching) return NULL;
    for (size_t i = 0; i < 6; i++)
    {
        if (caching_nodes[i].node->block_id == id)
        {
            return caching_nodes[i].node;
        }
    }
    return NULL;
}

static void remove_cache_node(const node_t* const node)
{
    for (size_t i = 0; i < 6; i++)
    {
        if (caching_nodes[i].node == node)
        {
            caching_nodes[i].node = NULL;
            caching_nodes[i].ptr_key = NULL;
        }
    }
}

static void __pri_list_reset_weight__(node_t* head)
{
    node_t* current_node = head->next;
    while(current_node != NULL)
    {
        current_node->weight = 0;
        current_node = current_node->next;
    }
}

static void __pri_list_caching__(node_t* head)
{
    if (!list_caching || list_cache_lock)
    {
        __pri_list_reset_weight__(head);
        return;
    }

    if (head == NULL || head->next == NULL) return;

    for (size_t i = 0; i < 6; i++)
    {
        caching_nodes[i].node->weight = 0;
    }

    for (node_t* current_node = head; current_node != NULL; current_node=current_node->next)
    {
        if (current_node->weight < 54)
        {
            if (caching_nodes[count_cache].node != NULL)
            {
                caching_nodes[count_cache].node->cached = SN_FLAG_UNSET;
            }
            caching_nodes[count_cache].node = current_node;
            caching_nodes[count_cache].ptr_key = current_node->data;
            caching_nodes[count_cache].node->weight = 0;
            caching_nodes[count_cache].node->cached = SN_FLAG_SET;
            (count_cache == 6) ? count_cache = 0 : count_cache++;
        }
    }

}

static void inc_time(node_t* node)
{
    if (tto_order == INT8_MAX)
    {
        tto_order = 0;
        __pri_list_caching__(node);
    }
    tto_order++;
}

static void inc_weight(node_t* head, node_t* node)
{
    if (node->weight < 60)
    {
        node->weight++;
        return;
    }
    __pri_list_caching__(head);
}

node_t* list_init()
{
    node_t* ou = malloc(sizeof(node_t));
    if (ou == NULL) sn_crash(SN_ERR_SYS_FAIL);
    ou->previous = NULL;
    ou->data = NULL;
    ou->size = 0;
    ou->tid = 0;
    ou->cached = SN_FLAG_UNSET;
    ou->extended_data = NULL;
    ou->block_id = 0;
    ou->weight = UINT8_MAX;
    ou->next = NULL;
    return ou;
}

node_t* list_index_to_last(node_t* head)
{
    if (head == NULL)
        return NULL;

    node_t* current_node = head;
    for (; current_node->next != NULL; current_node=current_node->next);
    return current_node;
}

node_t* list_add(node_t* head, void* data)
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    node_t* last = list_index_to_last(head);
    node_t* new_last = malloc(sizeof(node_t));

    if (!new_last) sn_crash(SN_ERR_SYS_FAIL);

    last->next = new_last;
    new_last->previous = last;
    new_last->data = data;
    new_last->size = 0;
    new_last->tid = (uint64_t)pthread_self();
    new_last->cached = SN_FLAG_UNSET;
    new_last->extended_data = NULL;
    new_last->block_id = 0;
    new_last->weight = 0;
    new_last->next = NULL;

    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    return new_last;
}

void list_free_all(node_t* head)
{
    pthread_mutex_lock(&list_mutex);
    while (head != NULL)
    {
        node_t* next = head->next; // Save the next node
        pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
        list_free(head);  // Free the node
        pthread_mutex_unlock(&list_mutex);  // lock the mutex
        head = next;              // Move to the next node
    }
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
}

size_t  list_len(node_t* head)
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    size_t size = 0;
    for (node_t* current_node = head; current_node != NULL; current_node=current_node->next)
    {
        size++;
    }
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    return size;
}


void list_free(node_t* node)
{
    if (node == NULL) return;

    pthread_mutex_lock(&list_mutex);  // Lock the mutex

    // If the node has a previous node, update its next pointer
    if (node->previous != NULL)
    {
        node->previous->next = node->next;
    }

    // If the node has a next node, update its previous pointer
    if (node->next != NULL)
    {
        node->next->previous = node->previous;
    }

    // Free the node itself
    remove_cache_node(node);
    if (node->extended_data != NULL)
        free(node->extended_data); //((sn_ext_data_generic_section_t*)node->extended_data)->free_obj(node->extended_data);

    free(node);
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
}


node_t* list_query(node_t* head, const void* const data)
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    node_t* cag = get_caching_node(data);
    if (cag)
    {
        pthread_mutex_unlock(&list_mutex);
        return cag;
    }

    for (node_t* current_node = head; current_node != NULL; current_node=current_node->next)
    {
        if (current_node->data == data)
        {
            inc_weight(head, current_node);
            inc_time(head);
            last_access_node = current_node;
            pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
            return current_node; // Found a match; exit early
        }
    }
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    return NULL;
}


node_t* list_query_by_id(node_t* head, uint16_t id)
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    node_t* cag = get_caching_node_by_id(id);
    if (cag)
    {
        pthread_mutex_unlock(&list_mutex);
        return cag;
    }

    for (node_t* current_node = head; current_node != NULL; current_node=current_node->next)
    {
        if (current_node->block_id == id)
        {
            inc_weight(head, current_node);
            inc_time(head);
            last_access_node = current_node;
            pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
            return current_node; // Found a match; exit early
        }
    }
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    return NULL;
}

void list_free_all_with_data(node_t* head)
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    node_t* current_node = head;

    // Traverse the list
    while (current_node != NULL)
    {
        node_t* next_node = current_node->next;  // Save the next node

        // Free the data inside the node (if it's not NULL)
        if (current_node->data != NULL)
        {
            free(current_node->data);
        }

        // Free the current node itself
        pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
        list_free(current_node);  // Free the node
        pthread_mutex_unlock(&list_mutex);  // lock the mutex

        current_node = next_node;  // Move to the next node
    }
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
}