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

#include "platform_independent/plat_threading.h"

#include <stdlib.h>
#include <string.h>
#include "libsafetynet_config.h"

#include <platform_independent/plat_allocators.h>

#include "sn_crash.h"

#ifdef SN_CONFIG_ENABLE_MUTEX
#   ifdef SN_ON_UNIX
#       include <pthread.h>
#   elif defined(SN_ON_WIN32)
#       include <windows.h>
#   else
#       error "Unsupported platform for plat_threading"
#   endif
#endif

struct plat_mutex_s
{
#ifdef SN_CONFIG_ENABLE_MUTEX
#   ifdef SN_ON_UNIX
    pthread_mutex_t plat_mutex;
#   elif defined(SN_ON_WIN32)
    HANDLE plat_mutex;
#   endif
    uint64_t locker_tid;
#else
    uint8_t pad; // This is here because C With an empty struct return's zero for sizeof
#endif
};

plat_mutex_c plat_mutex_new()
{
    plat_mutex_c self = plat_malloc(sizeof(plat_mutex_t));


    if (!self)
    {
        return NULL;
    }

    memset(self, 0, sizeof(plat_mutex_t));
#ifdef SN_CONFIG_ENABLE_MUTEX
#   ifdef SN_ON_UNIX
    if (pthread_mutex_init(&self->plat_mutex, NULL) != 0)
    {
        plat_free(self);
        return NULL;
    }
#   elif defined(SN_ON_WIN32)
    self->plat_mutex = CreateMutexW(NULL, FALSE, NULL);
    if (!self->plat_mutex)
    {
        free(self);
        return NULL;
    }
#   endif
#endif

    return self;
}

void plat_mutex_lock(plat_mutex_c self)
{
    if (!self) return;
#ifdef SN_CONFIG_ENABLE_MUTEX
    if (plat_getTid() == self->locker_tid) return;

#   ifdef SN_ON_UNIX
    pthread_mutex_lock(&self->plat_mutex);
#   elif defined(SN_ON_WIN32)
    WaitForSingleObject(self->plat_mutex, INFINITE);
#   endif
    self->locker_tid = plat_getTid();
#endif
}

void plat_mutex_unlock(plat_mutex_c self)
{
    if (!self) return;
#ifdef SN_CONFIG_ENABLE_MUTEX
#   ifdef SN_ON_UNIX
    pthread_mutex_unlock(&self->plat_mutex);
#   elif defined(SN_ON_WIN32)
    ReleaseMutex(self->plat_mutex);
#   endif
    self->locker_tid = 0;
#endif
}


void plat_mutex_destroy(plat_mutex_c self)
{
    if (!self) return;
#ifdef SN_CONFIG_ENABLE_MUTEX
#   ifdef SN_ON_UNIX
    pthread_mutex_destroy(&self->plat_mutex);
#   elif defined(SN_ON_WIN32)
    CloseHandle(self->plat_mutex);
#   endif
#endif

    plat_free(self);
}

uint64_t plat_getTid()
{
#ifdef SN_CONFIG_ENABLE_MUTEX
#   ifdef SN_ON_UNIX
    return (uint64_t)(uintptr_t)pthread_self();
#   elif defined(SN_ON_WIN32)
    return (uint64_t)GetCurrentThreadId();
#   endif
#else
    return 0;
#endif
}
