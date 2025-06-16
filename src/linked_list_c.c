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

#include "linked_list_c.h"

#include <stdlib.h>
#include <string.h>

#include "sn_crash.h"

#include "plat_threading.h"

#pragma region "linked_list_entry_c code"

linked_list_entry_c linked_list_entry_new(linked_list_entry_c previous, void* data, size_t size, uint64_t tid)
{
    linked_list_entry_c self = malloc(sizeof(linked_list_entry_t));

    if (self == NULL)
    {
        return NULL;
    }

    memset(self, 0, sizeof(linked_list_entry_t));

    //If previous is null leave it for manual linking
    if (previous != NULL)
    {
        previous->next = self;
    }

    self->data = data;
    self->size = size;
    self->tid = tid;

    return self;
}

linked_list_entry_c linked_list_entry_getPreviousEntry(const linked_list_entry_c self)
{
    if (self == NULL) return NULL;
    return self->previous;
}

void linked_list_entry_setPreviousEntry(linked_list_entry_c self, linked_list_entry_c new_previous)
{
    if (self == NULL) return;
    self->previous = new_previous;
}

void* linked_list_entry_getData(const linked_list_entry_c self)
{
    if (self == NULL) return NULL;
    return self->data;
}

void linked_list_entry_setData(linked_list_entry_c self, void* new_data)
{
    if (self == NULL) return;
    self->data = new_data;
}

size_t linked_list_entry_getSize(const linked_list_entry_c self)
{
    if (self == NULL) return 0;
    return self->size;
}

void linked_list_entry_setSize(linked_list_entry_c self, size_t new_size)
{
    if (self == NULL) return;
    self->size = new_size;
}

uint64_t linked_list_entry_getTid(const linked_list_entry_c self)
{
    if (self == NULL) return 0;
    return self->tid;
}

void linked_list_entry_setTid(linked_list_entry_c self, uint64_t new_tid)
{
    if (self == NULL) return;
    self->tid = new_tid;
}

linked_list_entry_c linked_list_entry_getNextEntry(const linked_list_entry_c self)
{
    if (self == NULL) return NULL;
    return self->next;
}

void linked_list_entry_setNextEntry(linked_list_entry_c self, linked_list_entry_c new_next)
{
    if (self == NULL) return;
    self->next = new_next;
}

void linked_list_entry_destroy(linked_list_entry_c self)
{
    if (self == NULL) return;

    // Reweaving the list
    if (self->previous != NULL) {
        self->previous->next = self->next;
    }

    if (self->next != NULL) {
        self->next->previous = self->previous;
    }

    free(self);
}

static SN_BOOL linked_list_entry_pri_isHead(linked_list_entry_c self)
{
    return self->isHead;
}

#pragma endregion


#pragma region "linked_list_c code"

linked_list_c linked_list_new()
{
    linked_list_c self = malloc(sizeof(linked_list_t));

    if (self == NULL)
    {
        sn_crash(SN_ERR_SYS_FAIL);
    }
    memset(self, 0, sizeof(linked_list_t));

    //heads of lists are dummies They should be treated as Slightly immutable

    self->head = linked_list_entry_new(NULL, NULL, 0, plat_getTid());

    if (self->head == NULL)
    {
        sn_crash(SN_ERR_SYS_FAIL);
    }

    self->head->isHead = SN_TRUE;
    self->lastEntry = self->head;
    self->firstEntry = self->head;

    return self;
}

void linked_list_destroy(linked_list_c self)
{
    if (self == NULL) return;
    linked_list_entry_c entry = self->lastEntry;

    while (entry != NULL)
    {
        linked_list_entry_c prev = entry->previous;
        linked_list_entry_destroy(entry);
        entry = prev;
    }

    free(self);
}

void linked_list_push(linked_list_c self, void* data, size_t size, uint64_t tid)
{
    if (!self) return;
    linked_list_entry_c new_enty = linked_list_entry_new(NULL, data, size, tid);

    if (new_enty == NULL)
    {
        sn_crash(SN_ERR_SYS_FAIL);
    }


    //If pushing to a blank list
    if (self->firstEntry->isHead == SN_TRUE)
    {
        self->head->next = new_enty;
        self->firstEntry = new_enty;
        self->lastEntry = new_enty;
        self->len++;
        return;
    }

    linked_list_entry_c temp_first = self->firstEntry;
    self->firstEntry = new_enty;

    temp_first->previous = new_enty;
    self->head->next = new_enty;

    new_enty->next = temp_first;
    self->len++;
}

linked_list_entry_c linked_list_peek(linked_list_c self)
{
    return self->lastEntry;
}

void linked_list_pop(linked_list_c self)
{
    linked_list_entry_c temp = self->lastEntry;
    self->lastEntry = self->lastEntry->previous;
    linked_list_entry_destroy(temp);
    self->len--;
}

size_t linked_list_getSize(linked_list_c self)
{
    return self->len;
}

linked_list_entry_c linked_list_forEach(linked_list_c self, linked_list_for_each_worker_f worker, void* generic_arg)
{
    if (self == NULL || worker == NULL) return NULL;
    linked_list_entry_c entry = self->firstEntry;
    if (linked_list_entry_pri_isHead(entry))
        return NULL;

    size_t i = 0;

    while (entry != NULL)
    {
        linked_list_entry_c temp = worker(self, entry, i++, generic_arg);
        if (temp != NULL)
        {
            return temp;
        }
        entry = entry->next;
    }
    return NULL;
}

static linked_list_entry_c linked_list_searchForPointer(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg)
{
    if (linked_list_entry_getData(ctx) == generic_arg)
    {
        return ctx;
    }
    return NULL;
}

SN_BOOL linked_list_hasPtr(linked_list_c self, void* key)
{
    if (!self || !key) return SN_FALSE;
    linked_list_entry_c temp = linked_list_forEach(self, &linked_list_searchForPointer, key);
    if (temp != NULL)
    {
        return SN_TRUE;
    }
    return SN_FALSE;
}

linked_list_entry_c linked_list_getByPtr(linked_list_c self, void* key)
{
    if (!self || !key) return NULL;
    linked_list_entry_c temp = linked_list_forEach(self, &linked_list_searchForPointer, key);
    self->lastAccess = temp;
    return temp;
}

static linked_list_entry_c linked_list_searchForIndex(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg)
{
    if (!generic_arg) return NULL;
    if (index == *((const size_t*)generic_arg))
    {
        return ctx;
    }
    return NULL;
}

linked_list_entry_c linked_list_getByIndex(linked_list_c self, size_t index)
{
    if (!self) return NULL;
    if (linked_list_getSize(self) < index) return NULL;

    linked_list_entry_c temp = linked_list_forEach(self, &linked_list_searchForIndex, &index);
    self->lastAccess = temp;

    return temp;
}


#pragma endregion
