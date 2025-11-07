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
// Created by tete on 10/20/2025.
//
#define __SN_DEBUG_CALLS__
#include "libsafetynet.h"
#include <gtest/gtest.h>
#include <cstdint>

TEST(SafetynetAllocatorTests, mAllocOfZeroErrorTest)
{
    EXPECT_EQ(sn_malloc(0), nullptr);
    EXPECT_NE(sn_get_last_error(), SN_ERR_OK);
    EXPECT_EQ(sn_get_last_error(), SN_ERR_BAD_SIZE);
    sn_reset_last_error();
}

TEST(SafetynetAllocatorTests, AllocateAndFree)
{
    std::uint8_t* buff = reinterpret_cast<std::uint8_t*>(sn_malloc(sizeof(std::uint8_t)));
    EXPECT_NE(buff, nullptr);
    EXPECT_NE(sn_get_last_error(), SN_ERR_BAD_ALLOC);
    EXPECT_EQ(sn_get_last_error(), SN_ERR_OK);
    sn_reset_last_error();

    sn_free(buff);
    EXPECT_NE(sn_get_last_error(), SN_ERR_NO_ADDER_FOUND);
    EXPECT_EQ(sn_get_last_error(), SN_ERR_OK);
    sn_reset_last_error();
}

TEST(SafetynetAllocatorTests, ReallocBlockToZeroEdgeCaseTest)
{
    std::uint8_t* p = reinterpret_cast<std::uint8_t*>(sn_malloc(1));
    EXPECT_NE(p, nullptr);
    EXPECT_NE(sn_get_last_error(), SN_ERR_BAD_ALLOC);

    std::uint8_t* q = reinterpret_cast<std::uint8_t*>(sn_realloc(p, 0));
    EXPECT_EQ(q, nullptr);
    EXPECT_EQ(sn_get_last_error(), SN_ERR_BAD_SIZE);
    EXPECT_NE(p, nullptr); // original pointer still valid

    sn_free(p);
    sn_reset_last_error();
}

TEST(SafetynetAllocatorTests, AllocFreeCycling)
{
    void* buff[10];
    for (std::size_t i = 0; i < 10; i++)
    {
        buff[i] = sn_malloc(40);
        EXPECT_NE(buff[i], nullptr);
        EXPECT_EQ(sn_get_last_error(), SN_ERR_OK);
        sn_reset_last_error();
    }

    for (std::size_t i = 0; i < 10; i++)
    {
        sn_free(buff[i]);
        EXPECT_EQ(sn_get_last_error(), SN_ERR_OK);
        sn_reset_last_error();
    }
}
