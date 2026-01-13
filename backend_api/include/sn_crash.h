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

#include <stdint.h>
#include "libsafetynet.h"

//Do not call this directly
SN_NO_RETURN void __sn__pri__crash__(sn_error_codes_e err, uint32_t line, const char* file, const char* func_call_name, const char* message);

/**
 * @brief Causes the whole program to crash
 * @param err The error code from the error enum
 */
#define sn_crash(err) __sn__pri__crash__(err, __LINE__, __FILE_NAME__, __func__, NULL)

/**
 * @brief Causes the whole program to crash
 * @param err The error code from the error enum
 * @param message Error message
 */
#define sn_crashExt(err, message) __sn__pri__crash__(err, __LINE__, __FILE_NAME__, __func__, message)


#define SN_ASSERT(boolean_expression) do { if (!(boolean_expression)) { __sn__pri__crash__(SN_ERR_ASSERT_FAILED, __LINE__, __FILE_NAME__, __func__, "(" #boolean_expression ") is not true"); } } while (0)

// SN_ASSERT is ALWAYS active. Use SN_DEBUG_ASSERT for debug-only checks.
#ifdef SN_CONFIG_DEBUG
#   define SN_DEBUG_ASSERT(boolean_expression) SN_ASSERT(boolean_expression)
#else
#   define SN_DEBUG_ASSERT(expr) do { (void)0; } while (0)
//#   define SN_DEBUG_ASSERT(expr) ((void)sizeof(expr))
#endif

#endif //SN_CRASH_H
