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

#include "libsafetynet.h"
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
    else
    {
        self->next = NULL;
        self->previous = NULL;
    }

    self->data = data;
    self->size = size;
    self->tid = tid;
    self->mutex = NULL;

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
    plat_mutex_lock(self->mutex);
    self->previous = new_previous;
    plat_mutex_unlock(self->mutex);
}

void* linked_list_entry_getData(const linked_list_entry_c self)
{
    if (self == NULL) return NULL;
    return self->data;
}

void linked_list_entry_setData(linked_list_entry_c self, void* new_data)
{
    if (self == NULL) return;
    plat_mutex_lock(self->mutex);
    self->data = new_data;
    plat_mutex_unlock(self->mutex);
}

size_t linked_list_entry_getSize(const linked_list_entry_c self)
{
    if (self == NULL) return 0;
    return self->size;

}

void linked_list_entry_setSize(linked_list_entry_c self, size_t new_size)
{
    if (self == NULL) return;
    plat_mutex_lock(self->mutex);
    self->size = new_size;
    plat_mutex_unlock(self->mutex);
}

uint64_t linked_list_entry_getTid(const linked_list_entry_c self)
{
    if (self == NULL) return 0;
    return self->tid;
}

void linked_list_entry_setTid(linked_list_entry_c self, uint64_t new_tid)
{
    if (self == NULL) return;
    plat_mutex_lock(self->mutex);
    self->tid = new_tid;
    plat_mutex_unlock(self->mutex);
}

uint16_t linked_list_entry_getBlockId(const linked_list_entry_c self)
{
    if (self == NULL) return 0;
    return self->block_id;
}

void linked_list_entry_setBlockId(linked_list_entry_c self, uint16_t new_id)
{
    if (self == NULL) return;
    plat_mutex_lock(self->mutex);
    self->block_id = new_id;
    plat_mutex_unlock(self->mutex);
}

linked_list_entry_c linked_list_entry_getNextEntry(const linked_list_entry_c self)
{
    if (self == NULL) return NULL;
    return self->next;
}

void linked_list_entry_setNextEntry(linked_list_entry_c self, linked_list_entry_c new_next)
{
    if (self == NULL) return;
    plat_mutex_lock(self->mutex);
    self->next = new_next;
    plat_mutex_unlock(self->mutex);
}

void linked_list_entry_destroy(linked_list_entry_c self)
{
    if (self == NULL) return;
    free(self);
}

static SN_BOOL linked_list_entry_pri_isHead(linked_list_entry_c self)
{
    return self->isHead;
}


static void linked_list_entry_pri_reweave(linked_list_entry_c entry)
{
    if (entry->next)
    {
        entry->next->previous = entry->previous;
        entry->next = NULL;
    }

    if (entry->previous)
    {
        entry->previous->next = entry->next;
        entry->previous = NULL;
    }
}

#pragma endregion


#pragma region "linked_list_c code"

linked_list_c linked_list_new()
{
    linked_list_c self = malloc(sizeof(linked_list_t));

    if (self == NULL)
    {
        sn_crash(SN_ERR_CATASTROPHIC);
    }
    memset(self, 0, sizeof(linked_list_t));

    //heads of lists are dummies They should be treated as Slightly immutable

    self->head = linked_list_entry_new(NULL, NULL, 0, plat_getTid());

    if (self->head == NULL)
    {
        sn_crash(SN_ERR_CATASTROPHIC);
    }

    self->head->isHead = SN_TRUE;
    self->lastEntry = self->head;
    self->firstEntry = self->head;
    self->mutex = plat_mutex_new();

    return self;
}

static linked_list_entry_c pri_listDestroyer(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg)
{
    linked_list_entry_destroy(ctx);
    return NULL;
}

void linked_list_destroy(linked_list_c self)
{
    if (self == NULL) return;

    linked_list_forEach(self, pri_listDestroyer, NULL);
    linked_list_entry_destroy(self->head);
    plat_mutex_destroy(self->mutex);
    free(self);
}

void linked_list_push(linked_list_c self, void* data, size_t size, uint64_t tid)
{
    if (!self) return;
    if (self->firstEntry == NULL) sn_crash(SN_ERR_CATASTROPHIC);
    linked_list_entry_c new_enty = linked_list_entry_new(NULL, data, size, tid);

    if (new_enty == NULL)
    {
        sn_crash(SN_ERR_CATASTROPHIC);
    }

    new_enty->mutex = self->mutex;

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
    plat_mutex_lock(self->mutex);
    linked_list_entry_c temp = self->lastEntry;
    self->lastEntry = self->lastEntry->previous;
    linked_list_entry_destroy(temp);
    self->len--;
    plat_mutex_unlock(self->mutex);
}

size_t linked_list_getSize(linked_list_c self)
{
    return self->len;
}

linked_list_entry_c linked_list_forEach(linked_list_c self, linked_list_for_each_worker_f worker, void* generic_arg)
{
    if (self == NULL || worker == NULL) return NULL;
    linked_list_entry_c entry = self->firstEntry;
    plat_mutex_lock(self->mutex);
    if (linked_list_entry_pri_isHead(entry))
        return NULL;

    size_t i = 0;

    while (entry != NULL)
    {
        linked_list_entry_c next = entry->next;
        linked_list_entry_c temp = worker(self, entry, i++, generic_arg);
        plat_mutex_lock(self->mutex); // Just in case the worker uses anything that unlocks the mutex
        if (temp != NULL)
        {
            plat_mutex_unlock(self->mutex);
            return temp;
        }
        entry = next;
    }
    plat_mutex_unlock(self->mutex);
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
    plat_mutex_lock(self->mutex);
    linked_list_entry_c temp = linked_list_forEach(self, &linked_list_searchForPointer, key);
    if (temp != NULL)
    {
        self->lastAccess = temp;
        plat_mutex_unlock(self->mutex);
        return SN_TRUE;
    }
    plat_mutex_unlock(self->mutex);
    return SN_FALSE;
}

SN_BOOL linked_list_hasId(linked_list_c self, uint16_t id)
{
    if (!self) return NULL;
    linked_list_entry_c temp = linked_list_getById(self, id);

    if (temp == NULL)
        return SN_TRUE;

    return SN_FALSE;
}

linked_list_entry_c linked_list_getByPtr(linked_list_c self, void* key)
{
    if (!self || !key) return NULL;
    linked_list_entry_c temp = linked_list_forEach(self, &linked_list_searchForPointer, key);
    if (temp) self->lastAccess = temp;
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
    if (temp) self->lastAccess = temp;

    return temp;
}

static linked_list_entry_c linked_list_searchForId(linked_list_c self, linked_list_entry_c ctx, size_t index, void* generic_arg)
{
    if (!generic_arg) return NULL;
    if (linked_list_entry_getBlockId(ctx) == *((const uint16_t*)generic_arg))
    {
        return ctx;
    }
    return NULL;
}

linked_list_entry_c linked_list_getById(linked_list_c self, uint16_t id)
{
    if (!self) return NULL;
    plat_mutex_lock(self->mutex);
    linked_list_entry_c temp = linked_list_forEach(self, &linked_list_searchForId, (uint16_t*)&id);
    if (temp)
    {
        self->lastAccess = temp;
    }
    return temp;
}


void linked_list_removeEntryByPtr(linked_list_c self, void* key)
{
    if (!self || !key) return;

    plat_mutex_lock(self->mutex);
    linked_list_entry_c entry = linked_list_forEach(self, linked_list_searchForPointer, key);

    if (entry)
    {
        if (self->firstEntry == entry && entry->next)
        {
            self->firstEntry = entry->next;
        }
        else sn_crash(SN_ERR_CATASTROPHIC);
        if (self->lastEntry == entry && entry->previous)
        {
            self->lastEntry = entry->previous;
        }
        linked_list_entry_pri_reweave(entry);
        linked_list_entry_destroy(entry);
        self->len--;
    }
    else sn_crash(SN_ERR_CATASTROPHIC);

    plat_mutex_unlock(self->mutex);
}


#pragma endregion
