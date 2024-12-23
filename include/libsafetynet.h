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

#pragma once
#ifndef LIB_SAFTY_NET_H
#define LIB_SAFTY_NET_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SN_ERR_OK = 0,
    SN_ERR_NULL_PTR = 5,
    SN_ERR_NO_SIZE = 10,
    SN_ERR_BAD_SIZE = 15,
    SN_ERR_BAD_ALLOC = 20,
    SN_ERR_NO_ADDER_FOUND = 30,
    SN_WARN_DUB_FREE = 80
} sn_error_codes_e;

typedef size_t sn_mem_address_t;
void* sn_malloc(size_t size);
void  sn_free(void* const ptr);
void* sn_register(void* const ptr);
size_t sn_query_size(void* const ptr);
uint64_t sn_query_tid(void* const ptr);
void* sn_register_size(void* ptr, size_t size);
sn_error_codes_e sn_get_last_error();
void sn_reset_last_error();

#ifdef __cplusplus
}
#endif

#endif