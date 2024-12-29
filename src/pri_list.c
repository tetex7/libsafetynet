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
#include <pthread.h>

pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;


static void __pri_list_reset_weight__(node_t* head)
{
    node_t* current_node = head->next;
    while(current_node != NULL)
    {
        current_node->weight = 0;
        current_node = current_node->next;
    }
}

static void __pri_list_reord__(node_t* head)
{
    if (head == NULL || head->next == NULL)
        return; // Nothing to reorder

    node_t* current = head;
    while (current != NULL)
    {
        node_t* next = current;
        if ((next != NULL && current->weight < next->weight) && current != head)
        {
            // Swap current and next nodes
            if (current->previous != NULL)
            {
                current->previous->next = next;
            }
            next->previous = current->previous;

            if (next->next != NULL)
            {
                next->next->previous = current;
            }
            current->next = next->next;

            next->next = current;
            current->previous = next;

            // If head is part of the swap, update it
            if (current == head)
            {
                head = next;
            }

            // Go back to the previous node to check again
            current = next;
        }
        else
        {
            // Move forward
            current = current->next;
        }
    }
    __pri_list_reset_weight__(head);
}

static void inc_time(node_t* node)
{
    if (node->tto_order == INT8_MAX)
    {
        node->tto_order = 0;
        __pri_list_reord__(node);
    }
    node->tto_order++;
}

static void inc_weight(node_t* head, node_t* node)
{
    if (node->weight < INT8_MAX-20)
    {
        node->weight++;
        return;
    }
    __pri_list_reord__(head);
}

node_t* list_init()
{
    node_t* ou = malloc(sizeof(node_t));
    ou->previous = NULL;
    ou->data = NULL;
    ou->size = 0;
    ou->tid = 0;
    ou->tto_order = 0;
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

    last->next = new_last;
    new_last->previous = last;
    new_last->data = data;
    new_last->size = 0;
    new_last->tid = (uint64_t)pthread_self();
    new_last->tto_order = 0;
    new_last->weight = 0;
    new_last->next = NULL;

    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
    return new_last;
}

void list_free_all(node_t *head)
{
    pthread_mutex_lock(&list_mutex);
    while (head != NULL)
    {
        node_t* next = head->next; // Save the next node
        free(head);               // Free the current node
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
    if (node == NULL)
        return;

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
    free(node);
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
}


node_t* list_query(node_t* head, void* data)
{
    pthread_mutex_lock(&list_mutex);  // Lock the mutex
    
    for (node_t* current_node = head; current_node != NULL; current_node=current_node->next)
    {
        if (current_node->data == data)
        {
            inc_weight(head, current_node);
            inc_time(head);
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
        free(current_node);  // Free the node

        current_node = next_node;  // Move to the next node
    }
    pthread_mutex_unlock(&list_mutex);  // Unlock the mutex
}