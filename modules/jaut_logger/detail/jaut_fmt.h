/**
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
                     ░░░░░██╗░█████╗░██╗░░░██╗████████╗
                     ░░░░░██║██╔══██╗██║░░░██║╚══██╔══╝
                     ░░░░░██║███████║██║░░░██║░░░██║░░░
                     ██╗░░██║██╔══██║██║░░░██║░░░██║░░░
                     ╚█████╔╝██║░░██║╚██████╔╝░░░██║░░░
                     ░╚════╝░╚═╝░░╚═╝░╚═════╝░░░░╚═╝░░░
                       JUCE Augmented Utility  Toolbox
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
    
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

    Copyright (c) 2022 ElandaSunshine
    ===============================================================

    @author Elanda
    @file   jaut_fmt.h
    @date   06, November 2022

    ===============================================================
 */
 
#pragma once



// Let's make it usable for our purposes
// We define this so that we don't have to link against it
#define FMT_HEADER_ONLY



//======================================================================================================================
//================================================ FMT (MIT) ===========================================================
//======================================================================================================================
//      
//      Copyright (c) 2012 - present, Victor Zverovich
//      
//      Permission is hereby granted, free of charge,
//      to any person obtaining a copy of this software and associated documentation files (the "Software"),
//      to deal in the Software without restriction,
//      including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
//      and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
//      subject to the following conditions:
//      
//      The above copyright notice and this permission notice shall be
//      included in all copies or substantial portions of the Software.
//      
//      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//      INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//      WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
//      OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//      
//      --- Optional exception to the license ---
//      
//      As an exception, if, as a result of your compiling your source code,
//      portions of this Software are embedded into a machine-executable object form of such source code,
//      you may redistribute such embedded portions in such object form without including the above copyright and permission notices.
//
//======================================================================================================================
//
//      FMT can be found here: https://github.com/fmtlib/fmt
//
//======================================================================================================================

#include <jaut_logger/detail/fmt/args.h>
#include <jaut_logger/detail/fmt/chrono.h>
#include <jaut_logger/detail/fmt/color.h>
#include <jaut_logger/detail/fmt/core.h>
#include <jaut_logger/detail/fmt/format.h>
#include <jaut_logger/detail/fmt/os.h>
#include <jaut_logger/detail/fmt/ostream.h>
#include <jaut_logger/detail/fmt/printf.h>
#include <jaut_logger/detail/fmt/ranges.h>
#include <jaut_logger/detail/fmt/std.h>
#include <jaut_logger/detail/fmt/xchar.h>

#include <juce_core/juce_core.h>

//======================================================================================================================
//================================================ FMT (MIT) ===========================================================
//======================================================================================================================


//======================================================================================================================
namespace fmt
{
    template<>
    struct formatter<juce::String> : formatter<std::string>
    {
        template<class FormatContext>
        auto format(const juce::String &string, FormatContext &ctx) const
            -> decltype(ctx.out())
        {
            return formatter<std::string>::format(string.toStdString(), ctx);
        }
    };
}
