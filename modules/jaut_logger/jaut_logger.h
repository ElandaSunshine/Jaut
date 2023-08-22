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
    @file   jaut_provider.h
    @date   13, February 2023

    ===============================================================
    BEGIN_JUCE_MODULE_DECLARATION

    ID:                 jaut_logger
    vendor:             ElandaSunshine
    version:            1.0.0
    name:               JAUT Data & Storage Classes
    description:        Defines facilities to log to different outputs in different ways and formats
    website:            https://github.com/elandasunshine/jaut
    license:            GPL v3
    minimumCppStandard: 17
    dependencies:       jaut_message

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once



// Dependencies
#include <jaut_message/jaut_message.h>

// Def
#include <jaut_logger/jaut_logger_define.h>

// Main
#include <jaut_logger/jaut_AbstractLogger.h>
#include <jaut_logger/jaut_BasicLogger.h>
#include <jaut_logger/jaut_FlushPolicy.h>
#include <jaut_logger/jaut_LogLevel.h>
#include <jaut_logger/jaut_LogMessage.h>

// Detail
#include <jaut_logger/detail/jaut_fmt.h>

// Exceptions
#include <jaut_logger/exception/jaut_LogIOException.h>
#include <jaut_logger/exception/jaut_LogRotationException.h>

// Formatters
#include <jaut_logger/format/jaut_ILogFormat.h>
#include <jaut_logger/format/jaut_LogFormatCallback.h>
#include <jaut_logger/format/jaut_LogFormatJson.h>
#include <jaut_logger/format/jaut_LogFormatPattern.h>
#include <jaut_logger/format/jaut_LogFormatXml.h>

// Rotation
#include <jaut_logger/rotation/jaut_LogRotationManager.h>

#include <jaut_logger/rotation/policies/jaut_PolicyCombined.h>
#include <jaut_logger/rotation/policies/jaut_PolicyDaily.h>
#include <jaut_logger/rotation/policies/jaut_PolicyPeriodic.h>
#include <jaut_logger/rotation/policies/jaut_PolicySizeLimit.h>
#include <jaut_logger/rotation/policies/jaut_RotationPolicy.h>

#include <jaut_logger/rotation/strategies/jaut_RotationStrategy.h>
#include <jaut_logger/rotation/strategies/jaut_StrategyPattern.h>

// Sinks
#include <jaut_logger/sink/jaut_ILogSink.h>
#include <jaut_logger/sink/jaut_LogSinkFile.h>
#include <jaut_logger/sink/jaut_LogSinkOstream.h>
#include <jaut_logger/sink/jaut_LogSinkRotatingFile.h>
