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
    @file   jaut_FlushPolicy.h
    @date   06, October 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogLevel.h>
#include <jaut_logger/jaut_LogMessage.h>

#include <jaut_core/define/jaut_Define.h>

#include <bitset>
#include <functional>



namespace jaut
{
    /** Determines the policy-type for when the buffer should be flushed to the output. */
    struct JAUT_API FlushPolicy
    {
        //==============================================================================================================
        enum
        {
            /**
             *  Flush the buffer after a message has been logged.
             *  <br><br>
             *  This is the only flag that can not be combined with other flags, as every incoming message will
             *  be printed instantly.<br>
             *  If combined with other flags, this will overwrite all the others.
             */
            Instant,
        
            /** Flush all messages after a specified interval. */
            Timed,
        
            /** Flush all messages whenever the buffer is full. */
            Filled,
        
            /** Flush all messages whenever a certain level has been logged. */
            Levelled,
        
            /** Determines through a callback whether a message triggers a flush. */
            Custom,
            
            // FUTURE might have additional policies at this point
            
            /** Flush manually */
            Manual
        };
        
        //==============================================================================================================
        using Bitset = std::bitset<Manual>;
        
        /** A function type for custom flushing behaviour. */
        using CustomFlushPolicy = std::function<bool(const LogMessage&)>;
        
        //==============================================================================================================
        /** A struct of data that describes the flushing behaviour for an ILogWorker. */
        struct Settings
        {
            /** The callback for custom flushing policies. */
            CustomFlushPolicy customPolicy = [](const LogMessage&) { return false; };
            
            /**
             *  The interval when the buffer should be flushed. (in seconds)
             *  <br><br>
             *  This value is invalid if it is negative and having a negative value is UB as the library's way of
             *  handling negative values might change in future.
             */
            int interval = 10;
            
            /** The level that should instruct the buffer to be flushed. */
            LogLevel::Value level = LogLevel::Warn;
            
            /**
             *  The policy modes for when the log buffer should be flushed to the output.
             *  @see jaut::FlushPolicy
             */
            Bitset policies = (1u << Instant);
            
            /** Whether to flush as soon as the worker is marked to be finalised. (likely due to destruction) */
            bool flushOnFinalisation { true };
        };
    };
}
