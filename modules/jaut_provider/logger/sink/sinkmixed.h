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
    @file   sinkmixed.h
    @date   31, March 2019

    ===============================================================
 */

#pragma once

namespace jaut
{
    /**
     *  A sink that allows for combining multiple sinks.
     *  @tparam CriticalSection The lock type to use
     */
    template<class CriticalSection = juce::DummyCriticalSection>
    class LogSinkMixed : public ILogSink
    {
    public:
        using SinkPtr = std::unique_ptr<ILogSink>;
        
        //==============================================================================================================
        /** Creates a default instance with no sinks. */
        LogSinkMixed() = default;
        
        /**
         *  Create an instance with the specified list of sinks.
         *  @param sinkObjects The list of sinks
         */
        LogSinkMixed(std::initializer_list<SinkPtr> sinkObjects)
            : sinks{sinkObjects}
        {}
        
        /**
         *  Create an instance with the specified range of sinks.
         *
         *  @tparam InputIt The range iterator
         *  @param first The beginning of the range
         *  @param last  The end of the range
         */
        template<class InputIt>
        LogSinkMixed(InputIt first, InputIt last)
            : sinks(first, last)
        {}
        
        //==============================================================================================================
        void print(const juce::String &message, Logger::LogLevel level) final
        {
            typename CriticalSection::ScopedLockType lock(criticalSection);
            std::for_each(sinks.begin(), sinks.end(), [&message, level](auto &&sink) { sink->print(message, level); });
        }
        
        void flush() final
        {
            typename CriticalSection::ScopedLockType lock(criticalSection);
            std::for_each(sinks.begin(), sinks.end(), [](auto &&sink) { sink->flush(); });
        }
        
        //==============================================================================================================
        /**
         *  Adds another sink to the queue.
         *  @param sink The sink to add
         */
        void addSink(SinkPtr sink)
        {
            typename CriticalSection::ScopedLockType lock(criticalSection);
            sinks.push_back(std::move(sink));
        }
        
        /**
         *  Gets the vector of sinks this sink manages.
         *  @return The vector of sinks
         */
        std::vector<SinkPtr>& getSinks() noexcept
        {
            return sinks;
        }
        
        /**
         *  Gets the vector of sinks this sink manages.
         *  @return The vector of sinks
         */
        const std::vector<SinkPtr>& getSinks() const noexcept
        {
            return sinks;
        }
        
    private:
        CriticalSection criticalSection;
        std::vector<SinkPtr> sinks;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSinkMixed)
    };
    
    /**
     *  A sink that allows for combining multiple sinks.
     *
     *  @tparam CriticalSection The lock type to use
     *  @tparam Sinks           A list of sinks to mix
     */
    template<class CriticalSection, class ...Sinks>
    class LogSinkMixedStatic : public ILogSink
    {
    public:
        /**
         *  Creates a new instance of this sink.
         *  The arguments are for each sink that was passed into the Sinks template argument, in order.
         *
         *  @tparam Args The argument types of the arguments
         *  @param args The arguments to pass to the sinks
         */
        template<class ...Args>
        explicit LogSinkMixedStatic(Args &&...args)
            : sinks(std::make_tuple<Args...>(std::forward<Args>(args)...))
        {}
        
        //==============================================================================================================
        void print(const juce::String &message, Logger::LogLevel level) final
        {
            typename CriticalSection::ScopedLockType lock(criticalSection);
            (std::get<Sinks>(sinks).print(message, level), ...);
        }
        
        void flush() final
        {
            typename CriticalSection::ScopedLockType lock(criticalSection);
            (std::get<Sinks>(sinks).flush(), ...);
        }
        
        //==============================================================================================================
        template<class Sink>
        constexpr Sink& getSink() noexcept
        {
            return std::get<Sink>(sinks);
        }
        
        constexpr std::array<std::reference_wrapper<ILogSink>, sizeof...(Sinks)> getSinks() noexcept
        {
            return { (std::get<Sinks>(sinks), ...) };
        }
    
        constexpr std::array<std::reference_wrapper<const ILogSink>, sizeof...(Sinks)> getSinks() const noexcept
        {
            return { (std::get<Sinks>(sinks), ...) };
        }
        
    private:
        CriticalSection criticalSection;
        std::tuple<Sinks...> sinks;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSinkMixedStatic)
    };
}