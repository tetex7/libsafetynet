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
    void(*free_obj)(void* ext_data);
    void(*free_block)(void* block);
} __attribute__((aligned(8))) sn_ext_data_generic_section_t;

#define SN_GENER_EXT_DATA_INIT(type, struct_type, ext_start_data, free_obj, free_block) ((sn_ext_data_generic_section_t){ type, sizeof(struct_type), offsetof(struct_type, ext_start_data), free_obj, free_block })

typedef struct sn_mmap_ext_data_t
{
    sn_ext_data_generic_section_t generic_section;
    size_t size;
    int mmap_handle;
} __attribute__((aligned(8))) sn_mmap_ext_data_t;

void mmap_data_free(void* ptr);

typedef struct sn_file_ext_data_t
{
    sn_ext_data_generic_section_t generic_section;
    size_t size;
    const char* path;
    int file_handle;
    int mmap_handle;
    void* map_block;
} __attribute__((aligned(8))) sn_file_ext_data_t;

void ext_map_file_data_free(void* ptr);
void ext_obj_map_file_free(void* ptr);

#define FILE_MAP_EXT_DATA_INIT(_size, path, file_handle, mmap_handle, map_block) ((sn_file_ext_data_t){ SN_GENER_EXT_DATA_INIT(SN_MMAP_DATA, sn_mmap_ext_data_t, size, &ext_obj_map_file_free, &ext_map_file_data_free), _size, path, file_handle, mmap_handle, map_block })


#endif //EXTENDED_DATA_H
