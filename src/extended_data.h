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
// Created by tete on 01/11/2025.
//

#ifndef EXTENDED_DATA_H
#define EXTENDED_DATA_H
#include "_pri_api.h"

typedef struct sn_ext_data_generic_section_t
{
    uint8_t type;
    size_t data_size;
    uint16_t offset;
} __attribute__((aligned(8))) sn_ext_data_generic_section_t;

typedef struct sn_mmap_ext_data_t
{
    sn_ext_data_generic_section_t generic_section;
    size_t size;
    int mmap_handle;
} __attribute__((aligned(8))) sn_mmap_ext_data_t;

#define MMAP_EXT_DATA_INIT ((sn_mmap_ext_data_t){ { SN_MMAP_DATA, sizeof(mmap_ext_data_t), offsetof(mmap_ext_data_t, mmap_handle) } 0 })


#endif //EXTENDED_DATA_H
