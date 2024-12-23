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

#include <stdexcept>
#undef new
#undef delete
#include "libsafetynet.h"


void* operator new(size_t size) {
    void* ptr = sn_malloc(size);  // Use custom malloc
    if (!ptr) {
        throw std::bad_alloc();
    }
    return ptr;
}

void operator delete(void* ptr) noexcept {
    sn_free(ptr);  // Use custom free
}

void* operator new[](size_t size) {
    return ::operator new(size);  // Can be expanded for array-specific handling
}

void operator delete[](void* ptr) noexcept {
    ::operator delete(ptr);  // Can be expanded for array-specific handling
}
