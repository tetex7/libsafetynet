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
// Created by tete on 06/16/2025.
//

#include "plat_threading.h"

#include <stdlib.h>
#include <string.h>

#include "sn_crash.h"


#ifdef __unix
#   include <pthread.h>
#elif defined(_WIN32)
#   include <windows.h>
#else
#   error "Unsupported platform for plat_threading"
#endif

struct plat_mutex_s
{
#ifdef __unix
    pthread_mutex_t plat_mutex;
#elif defined(_WIN32)
    HANDLE plat_mutex;
#endif
};

plat_mutex_c plat_mutex_new()
{
    plat_mutex_c self = malloc(sizeof(plat_mutex_t));

    if (!self)
    {
        return NULL;
    }
#ifdef __unix
    if (pthread_mutex_init(&self->plat_mutex, NULL) != 0)
    {
        free(self);
        return NULL;
    }
#elif defined(_WIN32)
    self->plat_mutex = CreateMutexW(NULL, FALSE, NULL);
    if (!self->plat_mutex)
    {
        free(self);
        return NULL;
    }
#endif
    return self;
}

void plat_mutex_lock(plat_mutex_c self)
{
    if (!self) return;

#ifdef __unix
    pthread_mutex_lock(&self->plat_mutex);
#elif defined(_WIN32)
    WaitForSingleObject(self->plat_mutex, INFINITE);
#endif
}

void plat_mutex_unlock(plat_mutex_c self)
{
    if (!self) return;

#ifdef __unix
    pthread_mutex_unlock(&self->plat_mutex);
#elif defined(_WIN32)
    ReleaseMutex(self->plat_mutex);
#endif
}


void plat_mutex_destroy(plat_mutex_c self)
{
    if (!self) return;

#ifdef __unix
    pthread_mutex_destroy(&self->plat_mutex);
#elif defined(_WIN32)
    CloseHandle(self->plat_mutex);
#endif

    free(self);
}

uint64_t plat_getTid()
{
#ifdef __unix
    return (uint64_t)(uintptr_t)pthread_self();
#elif defined(WIN32)
    return (uint64_t)GetCurrentThreadId();
#endif
}
