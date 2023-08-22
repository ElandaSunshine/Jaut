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
    @file   jaut_LogSinkOstream.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogMessage.h>
#include <jaut_logger/sink/jaut_ILogSink.h>

#include <jaut_core/define/jaut_Define.h>

#include <iostream>



//**********************************************************************************************************************
// region Header
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    /**
     *  A Logger output sink for the standard std::ostream.
     *  This can be std::cout, std::cerr, std::clog or any other std::ostream.
     *  <br><br>
     *  This sink does not support replacing formatters.
     */
    template<class CriticalSection = juce::DummyCriticalSection>
    class JAUT_API LogSinkOstream : public ILogSink
    {
    public:
        static constexpr bool default_lock = false;
        
        //==============================================================================================================
        /**
         *  Creates a new std::ostream log sink.
         *  
         *  @param ostream   The std::ostream to manage
         *  @param formatter The formatter used to format messages
         */
        explicit LogSinkOstream(std::ostream                &ostream,
                                std::unique_ptr<ILogFormat> formatter
                                    = std::make_unique<DefaultFormatterType>()) noexcept;
        
        //==============================================================================================================
        void print(const LogMessage &message) override;
        
        void flush() override;
        
        //==============================================================================================================
        void onOpen() override;
        
        void onClose() override;
        
        //==============================================================================================================
        /**
         *  Gets the sink's lock if one was used.
         *  You will want to acquire this lock when you are trying to read the stream on another thread than the
         *  printing and flushing happens on.
         *  
         *  @return This sink's lock
         */
        JAUT_NODISCARD
        CriticalSection &getStreamLock() const noexcept;
        
        /**
         *  Acquires the underlying stream of this sink.
         *  @return The std::ostream object reference
         */
        JAUT_NODISCARD
        std::ostream &getManagedStream() noexcept;
        
        /**
         *  Acquires the underlying stream of this sink.
         *  @return The std::ostream object reference
         */
        JAUT_NODISCARD
        const std::ostream &getManagedStream() const noexcept;
        
        JAUT_NODISCARD
        const ILogFormat *getFormatter() const override;
    
    private:
        using LockGuard = typename CriticalSection::ScopedLockType;
        
        //==============================================================================================================
        mutable CriticalSection lock;
        std::ostream &ostream;
        std::unique_ptr<ILogFormat> formatter;
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSinkOstream)
    };
    
    //==================================================================================================================
    namespace detail
    {
        //==============================================================================================================
        template<std::ostream &Stream>
        class LogSinkStandard : public LogSinkOstream<juce::DummyCriticalSection>
        {
        public:
            explicit LogSinkStandard(std::unique_ptr<ILogFormat> formatter
                                         = std::make_unique<DefaultFormatterType>()) noexcept;
        };
        
        //==============================================================================================================
        template<std::ostream &S>
        inline LogSinkStandard<S>::LogSinkStandard(std::unique_ptr<ILogFormat> parFormatter) noexcept
            : LogSinkOstream(S, std::move(parFormatter))
        {}
    }
    
    //==================================================================================================================
    using LogSinkCout = detail::LogSinkStandard<std::cout>;
    using LogSinkCerr = detail::LogSinkStandard<std::cerr>;
    using LogSinkClog = detail::LogSinkStandard<std::clog>;
}
//======================================================================================================================
// region Header
//**********************************************************************************************************************
// region Implementation (LogSinkOstream)
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    template<class CS>
    inline LogSinkOstream<CS>::LogSinkOstream(std::ostream                &parOstream,
                                              std::unique_ptr<ILogFormat> parFormatter) noexcept
        : ostream  (parOstream),
          formatter(std::move(parFormatter))
    {}
    
    //==================================================================================================================
    template<class CS>
    inline void LogSinkOstream<CS>::print(const jaut::LogMessage &parMessage)
    {
        jdscoped LockGuard(lock);
        
        if (formatter)
        {
            ostream << formatter->format(parMessage);
        }
        else
        {
            ostream << parMessage.message;
        }
    }
    
    template<class CS>
    inline void LogSinkOstream<CS>::flush()
    {
        jdscoped LockGuard(lock);
        ostream << std::flush;
    }
    
    //==================================================================================================================
    template<class CS>
    void LogSinkOstream<CS>::onOpen()
    {
        ILogFormat::Util::printHeader(getFormatter(), [&out = ostream](const juce::String &header)
        {
            out << header << std::endl;
        });
    }
    
    template<class CS>
    void LogSinkOstream<CS>::onClose()
    {
        ILogFormat::Util::printFooter(getFormatter(), [&out = ostream](const juce::String &footer)
        {
            out << footer << std::endl;
        });
    }
    
    //==================================================================================================================
    template<class CS>
    inline CS& LogSinkOstream<CS>::getStreamLock() const noexcept
    {
        return lock;
    }
    
    template<class CS>
    std::ostream& LogSinkOstream<CS>::getManagedStream() noexcept
    {
        return ostream;
    }
    
    template<class CS>
    const std::ostream& LogSinkOstream<CS>::getManagedStream() const noexcept
    {
        return ostream;
    }
    
    template<class CS>
    inline const ILogFormat* LogSinkOstream<CS>::getFormatter() const
    {
        return formatter.get();
    }
}
//======================================================================================================================
// region Implementation (LogSinkOstream)
//**********************************************************************************************************************
