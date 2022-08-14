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
    @file   sinkfmt.h
    @date   31, March 2022

    ===============================================================
 */

#pragma once

namespace jaut
{
    /**
     *  A Logger output sink for the standard output stream using fmt for colouring messages depending on their level.
     *  @tparam CriticalSection The lock type to use
     */
    template<class CriticalSection = juce::DummyCriticalSection>
    class LogSinkFmtStdout : public detail::LogSinkFmtImpl<CriticalSection>
    {
    public:
        LogSinkFmtStdout()
            : detail::LogSinkFmtImpl<CriticalSection>(stdout)
        {}
    };
    
    /**
     *  A Logger output sink for the standard error stream using fmt for colouring messages depending on their level.
     *  @tparam CriticalSection The lock type to use
     */
    template<class CriticalSection = juce::DummyCriticalSection>
    class LogSinkFmtStderr : public detail::LogSinkFmtImpl<CriticalSection>
    {
    public:
        LogSinkFmtStderr()
            : detail::LogSinkFmtImpl<CriticalSection>(stderr)
        {}
    };
}