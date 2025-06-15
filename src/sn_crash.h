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

#ifndef SN_CRASH_H
#define SN_CRASH_H

#include "libsafetynet.h"
#define SN_NO_RET __attribute__ ((__noreturn__))
SN_VERY_OPTIMIZED SN_NO_RET void __sn__pri__crash__(sn_error_codes_e err, uint32_t line, const char* file); //__attribute__ ((__noreturn__));

#define sn_crash(err) __sn__pri__crash__(err, __LINE__, __FILE__);

#endif //SN_CRASH_H
