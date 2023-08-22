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
    @file   jaut_RotationStrategy.h
    @date   26, January 2023

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    class LogRotationManager;
    
    //==================================================================================================================
    /**
     *  The rotation strategy.<br>
     *  The strategy will determine how log files are rotated and where and under what name they will end up.
     *  For simple strategies, a lambda is more than enough but can be extended to a functor class for
     *  a more-configurable approach.
     *  <br><br>
     *  If this is used in conjunction with jaut::SinkRotatingFile,
     *  you do not need to take care of closing/deleting the main log file.
     *  
     *  @param currentLogFile The log rotation manager that this callback is tied to
     *  @return The newly rotated archive file
     */
    JAUT_API using RotationStrategy = std::function<juce::File(const LogRotationManager &rotationManager)>;
}
