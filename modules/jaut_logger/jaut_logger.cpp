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
    @file   jaut_provider.cpp
    @date   20, March 2020
    
    ===============================================================
 */

#include <jaut_logger/jaut_logger.h>



// Main
#include <jaut_logger/jaut_AbstractLogger.cpp>

// Builder
#include "jaut_logger/builder/factory/jaut_FactoryNode.cpp"

// Formatters
#include <jaut_logger/format/jaut_LogFormatCallback.cpp>
#include <jaut_logger/format/jaut_LogFormatJson.cpp>
#include <jaut_logger/format/jaut_LogFormatPattern.cpp>
#include <jaut_logger/format/jaut_LogFormatXml.cpp>

// Rotation
#include <jaut_logger/rotation/jaut_LogRotationManager.cpp>

#include <jaut_logger/rotation/policies/jaut_PolicyDaily.cpp>
#include <jaut_logger/rotation/policies/jaut_PolicyPeriodic.cpp>
#include <jaut_logger/rotation/policies/jaut_PolicySizeLimit.cpp>

#include <jaut_logger/rotation/strategies/jaut_StrategyPattern.cpp>

// Sinks
#include <jaut_logger/sink/jaut_LogSinkFile.cpp>
#include <jaut_logger/sink/jaut_LogSinkRotatingFile.cpp>
