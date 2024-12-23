#pragma once
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>  // For size_t
#include <stdlib.h>  // For malloc, free
#include <stdbool.h>

// Define the linked list node structure
typedef struct node_t {
    struct node_t* previous;     // Pointer to the next node
    void* data;            // Pointer to data (generic data type)
    size_t size;             //size of the data
    struct node_t* next;     // Pointer to the next node
} node_t;

node_t* list_init();
node_t* list_query(node_t* head, void* data);
size_t  list_len(node_t* head);
node_t* list_index_to_last(node_t* head);
node_t* list_add(node_t* head, void* data);
void list_free_all(node_t *head);
void list_free(node_t* node);
void list_free_all_with_data(node_t* head);



#endif // LINKED_LIST_H