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
    @file   sinkfile.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

namespace jaut
{
    /**
     *  A Logger output sink for log files.
     *  When using the juce::InterProcessLock as the lock type, the file name will be used as lock-identifier.
     *
     *  @tparam RotationHandler The rotation handler
     *  @tparam CriticalSection The lock type to use
     */
    template<class RotationHandler, class CriticalSection = juce::DummyCriticalSection>
    using LogSinkFile = detail::LogSinkFileImpl<CriticalSection, RotationHandler>;
}