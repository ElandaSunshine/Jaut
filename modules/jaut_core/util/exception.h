/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   exception.h
    @date   19, August 2019
    
    ===============================================================
 */

#pragma once

#define JAUT_CREATE_EXCEPTION(NAME, ERROR_MSG) \
             class JAUT_API x final : public jaut::Exception { \
             public: NAME() noexcept : jaut::Exception(juce::String(ERROR_MSG)) {}}

#define JAUT_CREATE_EXCEPTION_WITH_STRING(NAME, ERROR_MSG_TITLE) \
             class JAUT_API x final : public jaut::Exception { \
             public: explicit NAME(const juce::String &message) noexcept \
             : jaut::Exception(juce::String(ERROR_MSG_TITLE) + message) {}}

namespace jaut
{
class JAUT_API Exception : public std::exception
{
public:
    explicit Exception(juce::String msg) noexcept
        : message(std::move(msg))
    {}

    //==================================================================================================================
    const char *what() const noexcept override
    {
        return message.toRawUTF8();
    }

private:
    juce::String message;
};
}
