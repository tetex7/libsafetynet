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

//
// Created by tete on 01/06/2025.
//

#include "libsafetynet.h"

SN_PUB_API_OPEN void* operator new(size_t size)
{
    void* p = sn_malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}

SN_PUB_API_OPEN void operator delete(void* p) noexcept
{
    sn_free(p);
}

SN_PUB_API_OPEN void* operator new[](size_t size)
{
    void* p = sn_malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}

SN_PUB_API_OPEN void operator delete[](void* p) noexcept
{
    sn_free(p);
}


