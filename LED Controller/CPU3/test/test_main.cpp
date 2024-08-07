
/*
    Light Tower by Rob Shockency
    Copyright (C) 2021 Rob Shockency degnarraer@yahoo.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version of the License, or
    (at your option) any later version. 3

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "AllTests.h"

#if defined(ARDUINO)
#include <Arduino.h>

void setup()
{
    Serial.begin(500000);
    ::testing::InitGoogleMock();
    ::testing::InitGoogleTest();
    ::testing::GTEST_FLAG(break_on_failure) = false;
}

void loop()
{
    RUN_ALL_TESTS();
    Serial.flush();
    delay(1000);
}

#else
int main(int argc, char **argv)
{
    ::testing::InitGoogleMock(&argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return result;
}
#endif