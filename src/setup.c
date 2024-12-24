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
#include <stdio.h>

#include "libsafetynet.h"
#include <stdio.h>
#include <stdlib.h>
#include "pri_list.h"
#include "pthread.h"
#include "_pri_api.h"

node_t* mem_list = NULL;

sn_error_codes_e error_code = SN_ERR_OK;

extern pthread_mutex_t list_mutex;
void doexit()
{
    if (mem_list != NULL) {
        list_free_all_with_data(mem_list);  // Free all the nodes and their data
        mem_list = NULL;  // Optionally set mem_list to NULL to prevent accidental use
        pthread_mutex_destroy(&list_mutex);
        pthread_mutex_destroy(&last_error_mutex);
    }
}

// Constructor: Called when the library is loaded
__attribute__((constructor))
void library_init() {
    mem_list = list_init();
    //atexit(doexit);
}



// Destructor: Called when the library is unloaded
__attribute__((destructor))
void library_cleanup() {
    // Perform any necessary cleanup, such as freeing global memory or other resources.
    // This function is called when the shared library is unloaded.
    //list_free_all(mem_list);
    doexit();
}

void sn_set_last_error(sn_error_codes_e er_code)
{
    pthread_mutex_lock(&last_error_mutex);
    error_code = er_code;
    pthread_mutex_unlock(&last_error_mutex);
}
