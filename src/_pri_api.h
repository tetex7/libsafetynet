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
#include "pthread.h"
extern pthread_mutex_t last_error_mutex;

extern sn_error_codes_e error_code;
__inline void sn_set_last_error(sn_error_codes_e er_code)
{
    pthread_mutex_lock(&last_error_mutex);
    error_code = er_code;
    pthread_mutex_unlock(&last_error_mutex);
}


#endif