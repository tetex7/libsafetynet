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
#ifndef _PRI_API_H
#define _PRI_API_H
#include "linked_list_c.h"
#include "plat_threading.h"

SN_PUB_API_OPEN
void sn_set_last_error(const sn_error_codes_e err);

extern linked_list_c mem_list;
extern plat_mutex_c alloc_mutex;

#define sn_error(errorCode, ...) \
{ \
    sn_set_last_error(errorCode); \
    return __VA_ARGS__; \
}

#endif //_PRI_API_H
