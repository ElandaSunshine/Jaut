/**
 * ===============================================================
 * This file is part of the Esac-Jaut library.
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
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2019 ElandaSunshine
 * ===============================================================
 *
 * Author: Elanda
 * File: exception.h
 * Time: 19, August 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>

#define JAUT_CREATE_EXCEPTION(x, y) class x final : public jaut::Exception { \
public: x() noexcept : jaut::Exception(String(y)) {}}

#define JAUT_CREATE_EXCEPTION_WITH_STRING(x, y) class x final : public jaut::Exception { \
public: x(const String &message) noexcept : jaut::Exception(String(y) + message) {}}

namespace jaut
{
class JAUT_API Exception : public std::exception
{
public:
    Exception(const String &msg) noexcept
        : message(msg)
    {}

    const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override
    {
        return message.toRawUTF8();
    }

private:
    String message;
};

namespace exception
{
/**
 * Occurs when trying to dereference a null-pointer.
 */
JAUT_CREATE_EXCEPTION_WITH_STRING(NullDereference, "Exception while trying to dereference a nullpointer object: ");
JAUT_CREATE_EXCEPTION(AppDataRootCreation, "Main data folder couldn't be created.\n"
                                           "Please report this issue to support for more"
                                           "information on how to solve this issue!");
}

#undef JAUT_CREATE_EXCEPTION
#undef JAUT_CREATE_EXCEPTION_WITH_STRING
}
