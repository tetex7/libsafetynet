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
// Created by tete on 11/03/2025.
//
#include "libsafetynet.h"

#ifdef SN_CONFIG_VERBOSE_LOGGING_FACILITIES_FEATURE
SN_PUB_API_OPEN SN_BOOL sn_enable_verbose_logging(const char* path)
{
    return SN_TRUE;
}
SN_PUB_API_OPEN SN_BOOL sn_disable_verbose_logging()
{
    return SN_TRUE;
}
#endif
