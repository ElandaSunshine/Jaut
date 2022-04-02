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

    @author Elanda (elanda@elandasunshine.com)
    @file   sinkfmtimpl.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

#include <fmt/core.h>
#include <fmt/color.h>

namespace jaut::detail
{
    template<class CriticalSection>
    class LogSinkFmtImpl : public ILogSink
    {
    public:
        explicit LogSinkFmtImpl(std::FILE *const file)
                : stream(file)
        {}
        
        //==============================================================================================================
        void print(const juce::String &message, Logger::LogLevel level) final
        {
            static constexpr std::array colours {
                    fmt::color::white,
                    fmt::color::orange,
                    fmt::color::indian_red,
                    fmt::color::lime_green,
                    fmt::color::indian_red
            };
            
            const typename CriticalSection::ScopedLockType lock(criticalSection);
            
            if (level != Logger::None)
            {
                fmt::print(stream, fmt::fg(colours[static_cast<std::size_t>(getBitPosition(level))]),
                           message.toStdString() + '\n');
            }
            else
            {
                fmt::print(stream, message.toStdString() + '\n');
            }
        }
        
        void flush() final
        {
            const typename CriticalSection::ScopedLockType lock(criticalSection);
            std::fflush(stream);
        }
    
    private:
        static constexpr int getBitPosition(jaut::Logger::LogLevel level)
        {
            return static_cast<int>(std::log2(static_cast<double>(level)));
        }
    
        //==============================================================================================================
        CriticalSection criticalSection;
        std::FILE       *stream;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSinkFmtImpl)
    };
}