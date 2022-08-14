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

    @author Elanda (elanda@elandasunshine.com)
    @file   timedrotationhandler.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

namespace jaut
{
    /**
     *  A timed rotation handler, this will rotate the log file at the specified interval.
     *
     *  @tparam IntervalInSeconds The interval at which rotation should happen in seconds
     *  @tparam FileBased         If true, the start of the interval will be the creation of the file, otherwise,
     *                            the start of the interval will be the creation of this handler
     */
    template<int IntervalInSeconds, bool FileBased = true>
    class TimedRotationHandler
    {
    public:
        TimedRotationHandler()
            : begin(toSeconds(juce::Time::currentTimeMillis()))
        {}
        
        template<class = std::enable_if_t<FileBased>>
        explicit TimedRotationHandler(const juce::File &file)
            : begin(toSeconds(file.getCreationTime().toMilliseconds()))
        {}
        
        //==============================================================================================================
        bool needsToBeRotated(const juce::String&, Logger::LogLevel) const
        {
            const std::int64_t now = toSeconds(juce::Time::currentTimeMillis());
            return ((now - begin) >= IntervalInSeconds);
        }
        
        void reset()
        {
            begin = juce::Time::currentTimeMillis();
        }
        
    private:
        static constexpr std::int64_t toSeconds(std::int64_t time)
        {
            return static_cast<std::int64_t>(std::round(static_cast<double>(time) / 1000));
        }
        
        //==============================================================================================================
        std::int64_t begin;
    };
}