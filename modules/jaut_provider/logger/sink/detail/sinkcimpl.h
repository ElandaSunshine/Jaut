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
    @file   sinkcimpl.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

namespace jaut::detail
{
    template<std::ostream &Stream, class CriticalSection>
    class LogSinkStandardStreamImpl : public ILogSink
    {
    public:
        LogSinkStandardStreamImpl() = default;
        
        //==============================================================================================================
        void print(const juce::String &message, Logger::LogLevel) final
        {
            const juce::ScopedLock lock(criticalSection);
            Stream << message << '\n';
        }
        
        void flush() final
        {
            const juce::ScopedLock lock(criticalSection);
            std::flush(Stream);
        }
    
    private:
        CriticalSection criticalSection;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSinkStandardStreamImpl)
    };
}