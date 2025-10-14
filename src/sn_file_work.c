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
// Created by tete on 10/13/2025.
//

#include <stdio.h>

#include "sn_crash.h"
#include "_pri_api.h"

static SN_BOOL file_exists(const char* path)
{
    FILE* f = fopen(path, "rb"); // try to open for reading
    if (f) {
        fclose(f);
        return SN_TRUE; // file exists
    }
    return SN_FALSE; // file doesn't exist
}

SN_PUB_API_OPEN SN_FLAG sn_dump_to_file(const char* file, void* block)
{
    if (!file || !block)
    {
        sn_error(SN_ERR_NULL_PTR, 0);
    }

    if (file_exists(file))
    {
        sn_error(SN_ERR_FILE_PRE_EXIST, 0);
    }

    linked_list_entry_c entry = memman_TryCacheHit(memory_manager, block);

    if (entry == MEMMAN_CACHE_MISS)
    {
        entry = linked_list_getByPtr(mem_list, block);
        if (!entry)
            sn_error(SN_ERR_NO_ADDER_FOUND, 0);
    }

    FILE* f = fopen(file, "wb");
    if (!f) {
        sn_error(SN_ERR_FILE_IO, 0);
    }

    size_t written = fwrite(block, 1, linked_list_entry_getSize(entry), f);
    fclose(f);

    if (written != linked_list_entry_getSize(entry))
    {
        sn_error(SN_ERR_FILE_IO, 0);
    }

    return 1;
}

SN_PUB_API_OPEN void* sn_mount_file_to_ram(const char* file)
{
    if (!file)
    {
        sn_error(SN_ERR_NULL_PTR, NULL);
    }

    if (!file_exists(file))
    {
        sn_error(SN_ERR_FILE_NOT_EXIST, NULL);
    }

    FILE* f = fopen(file, "rb");
    if (!f)
    {
        sn_error(SN_ERR_FILE_IO, NULL);
    }

    fseek(f, 0, SEEK_END);
    const size_t filesize = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    if (filesize <= 0) {
        fclose(f);
        sn_error(SN_ERR_BAD_SIZE, NULL);
    }

    void* buffer = sn_malloc(filesize);
    if (!buffer)
    {
        fclose(f);
        sn_crash(SN_ERR_BAD_ALLOC);
    }

    size_t read = fread(buffer, 1, (size_t)filesize, f);
    fclose(f);

    if (read != filesize) {
        sn_free(buffer); // optional cleanup
        sn_error(SN_ERR_FILE_IO, NULL);
    }

    return buffer;
}
