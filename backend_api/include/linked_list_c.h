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
#pragma once

/*
 * This list has very little thread protections just enough not to blow yourself up on first go
 */

#ifndef LINKED_LIST_C_H
#define LINKED_LIST_C_H
#include <stdint.h>
#include <stddef.h>
#include "platform_independent/plat_threading.h"
#include "libsafetynet.h"

typedef struct linked_list_entry_s
{
    struct linked_list_entry_s* previous;
    void* data;           // Pointer to data (generic data type)
    size_t size;          // size of the data
    sn_tid_t tid;         // The tid of the thread that allocated this chunk
    SN_FLAG cached;       // The denotes if it's been cached by the memory manager
    uint16_t block_id;    // An optional block id
    // Do not create a getter nor a setter for this treat this as private
    SN_BOOL isHead;       // To be determined
    uint8_t _weight;      // For used for caching(private)
    plat_mutex_c mutex;   // A mutex inherited from the list container
    struct linked_list_entry_s* next;
} *linked_list_entry_c, linked_list_entry_t;

linked_list_entry_c linked_list_entry_new(linked_list_entry_c previous, void* data, size_t size, sn_tid_t tid);
linked_list_entry_c linked_list_entry_copy(linked_list_entry_c entry);

linked_list_entry_c linked_list_entry_getPreviousEntry(const linked_list_entry_c self);
void linked_list_entry_setPreviousEntry(linked_list_entry_c self, linked_list_entry_c new_previous);

void* linked_list_entry_getData(const linked_list_entry_c self);
void linked_list_entry_setData(linked_list_entry_c self, void* new_data);

size_t linked_list_entry_getSize(const linked_list_entry_c self);
void linked_list_entry_setSize(linked_list_entry_c self, size_t new_size);

uint64_t linked_list_entry_getTid(const linked_list_entry_c self);
void linked_list_entry_setTid(linked_list_entry_c self, uint64_t new_tid);

uint16_t linked_list_entry_getBlockId(const linked_list_entry_c self);
void linked_list_entry_setBlockId(linked_list_entry_c self, uint16_t new_id);

linked_list_entry_c linked_list_entry_getNextEntry(const linked_list_entry_c self);
void linked_list_entry_setNextEntry(linked_list_entry_c self, linked_list_entry_c new_next);

uint8_t linked_list_entry_pri_getWeight(linked_list_entry_c self);
void linked_list_entry_pri_setWeight(linked_list_entry_c self, uint8_t weight);

void linked_list_entry_destroy(linked_list_entry_c self);



typedef struct linked_list_s
{
    linked_list_entry_c head;
    size_t len;

    linked_list_entry_c firstEntry; //Physical beginning
    linked_list_entry_c lastEntry; //Physical last
    linked_list_entry_c lastAccess;
    plat_mutex_c mutex; // Shared by all elements within this list container
} *linked_list_c, linked_list_t;


#define LIST_FOR_EACH_LOOP_BRAKE ((linked_list_entry_c)-44)
typedef linked_list_entry_c(*linked_list_for_each_worker_f)(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg);

linked_list_c linked_list_new();

void linked_list_destroy(linked_list_c self);

void linked_list_push(linked_list_c self, void* data, size_t size, uint64_t tid);
linked_list_entry_c linked_list_peek(linked_list_c self);
void linked_list_pop(linked_list_c self);

linked_list_entry_c linked_list_getByPtr(linked_list_c self, void* key);
linked_list_entry_c linked_list_getByIndex(linked_list_c self, size_t index);
linked_list_entry_c linked_list_getById(linked_list_c self, uint16_t id);

SN_BOOL linked_list_hasPtr(linked_list_c self, void* key);
SN_BOOL linked_list_hasId(linked_list_c self, uint16_t id);

size_t linked_list_getSize(linked_list_c self);

linked_list_entry_c linked_list_forEach(linked_list_c self, linked_list_for_each_worker_f worker, void* generic_arg);

void linked_list_removeEntryByPtr(linked_list_c self, void* key);

SN_BOOL linked_list_removeEntry(linked_list_c self, linked_list_entry_c entry_ref);

#endif
