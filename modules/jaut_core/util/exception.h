/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any internal version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   exception.h
    @date   19, August 2019
    
    ===============================================================
 */

#pragma once

#define JAUT_CREATE_EXCEPTION(NAME, ERROR_MSG) \
             class JAUT_API NAME final : public std::runtime_error { \
             public: NAME() noexcept \
             : runtime_error(juce::String("Encountered " + juce::String(#NAME) + ": " + \
                                          juce::String(ERROR_MSG)).toStdString()) {}}

#define JAUT_CREATE_EXCEPTION_WITH_STRING(NAME, ERROR_MSG_TITLE) \
             class JAUT_API NAME final : public std::runtime_error { \
             public: explicit NAME(const juce::String &message) noexcept \
             : runtime_error(juce::String("Encountered " + juce::String(#NAME) + ": " + \
                                          juce::String(ERROR_MSG_TITLE) + message).toStdString()) {}}

/**
 *  Calls JUCE's jassert with an additional debug message to be printed.
 *
 *  @param COND The assertion condition that must be met
 *  @param MSG  The message to be printed if the condition wasn't met
 */
#define JAUT_ASSERT(COND, MSG) JUCE_BLOCK_WITH_FORCED_SEMICOLON(JAUT_DEBUGGING(if(!COND) DBG(MSG); jassertfalse;))

/**
 *  Forces an assert to fail.
 *  @param MSG The message to be printed for this assert
 */
#define JAUT_ASSERTFALSE(MSG) JAUT_ASSERT(false, MSG)

#if !DOXYGEN
#   define JAUT_THROWASSERT_1(MSG)            throwOrAssert(__LINE__, juce::String(__FILE__), MSG)
#   define JAUT_THROWASSERT_2(MSG, EXCEPTION) throwOrAssert<EXCEPTION>(__LINE__, juce::String(__FILE__), MSG)
#endif

/**
 *  Either asserts if in debug mode or throws an exception if in release mode.
 *  If you don't specify the custom exception type to be thrown, it will throw an std::runtime_error.
 *
 *  @param MSG       The message to be printed for this assert or exception
 *  @param EXCEPTION Which type of exception to throw (must be of type std::runtime_error)
 */
#define JAUT_THROWASSERT(...) JAUT_UPP_ARGS_OVERLOAD(JAUT_THROWASSERT, __VA_ARGS__)

namespace jaut
{
template<class Exception = std::runtime_error>
void throwOrAssert(int lineNumber, const juce::String &file, const juce::String &message)
{
    static_assert(std::is_base_of_v<std::runtime_error, Exception>,
                  "Type Exception must be of type std::runtime_error");
    
    const juce::String file_desc = "(" + file + ":" + juce::String(lineNumber) + ")";
    
    JAUT_DEBUGGING_OR
    (
        {
            DBG(message << file_desc);
            jassertfalse;
        }
    ,
        {
            throw Exception(message + file_desc);
        }
    )
}

namespace exception
{
JAUT_CREATE_EXCEPTION_WITH_STRING(InvalidVersionFormatException, "");
}
}
