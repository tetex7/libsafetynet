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
#ifndef _PRI_API_H
#define _PRI_API_H
#include "libsafetynet.h"
#include <pthread.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "pri_list.h"
extern pthread_mutex_t last_error_mutex;
extern pthread_mutex_t alloc_mutex;
extern pthread_mutex_t list_mutex;
extern sn_error_codes_e error_code;
extern SN_FLAG do_free_exit;
extern const char* const err_name_tap[];
extern const char* const human_readable_messages[];
extern node_t* mem_list;

extern sn_memalloc_call_t cus_memalloc_call;
extern sn_free_call_t cus_free_call;
extern sn_calloc_call_t cus_calloc_call;
extern sn_realloc_call_t cus_realloc_call;

void sn_set_last_error(sn_error_codes_e er_code);


typedef enum
{
    SN_MMAP_DATA = (uint8_t)10
} sn_ext_data_type_e;

#define SN_EXT_DATA_PREFORM \
uint8_t type; \
size_t data_size; \
uint16_t offset;




#endif