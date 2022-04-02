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
    @file   dailyrotationhandler.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

namespace jaut
{
    /**
     *  A daily rotation handler, this will rotate the log file everyday at the specified time in 24 hours format.
     *
     *  @tparam HourOfTheDay      The hour at which the log should rotate everyday
     *  @tparam MinuteOfTheHour   The minute of the hour of the day at which the log should rotate
     *  @tparam SecondOfTheMinute The second of the minute of the hour of the day at which the log should rotate
     */
    template<int HourOfTheDay, int MinuteOfTheHour, int SecondOfTheMinute = 0>
    class DailyRotationHandler
    {
    public:
        // Seems like you are trying to break time-and-space here
        // Make sure the values are directly proportional to a real-world's time understanding;
        // breaking through the border between dimensions is not included with this class unfortunately
        static_assert(HourOfTheDay      < 24 && HourOfTheDay      > -1, "Hours need to be a value between 0 and 23");
        static_assert(MinuteOfTheHour   < 60 && MinuteOfTheHour   > -1, "Minutes need to be a value between 0 and 59");
        static_assert(SecondOfTheMinute < 60 && SecondOfTheMinute > -1, "Seconds need to be a value between 0 and 59");
        
        //==============================================================================================================
        /** The specified time in seconds. (starting at 0 for midnight) */
        static constexpr int timeInSeconds = (HourOfTheDay * 3600) + (MinuteOfTheHour * 60) + SecondOfTheMinute;
        
        //==============================================================================================================
        bool needsToBeRotated(const juce::String&, Logger::LogLevel) const
        {
            const std::int64_t now = toSeconds(juce::Time::currentTimeMillis());
            return ((now % 86400) >= timeInSeconds);
        }
        
        void reset() {}
        
    private:
        static constexpr std::int64_t toSeconds(std::int64_t time)
        {
            return static_cast<std::int64_t>(std::round(static_cast<double>(time) / 1000));
        }
    };
}