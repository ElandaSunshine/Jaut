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
    @file   jaut_ILogWorker.h
    @date   03, October 2022

    ===============================================================
 */

#pragma once

#include <jaut_logger/jaut_LogMessage.h>
#include <jaut_logger/jaut_FlushPolicy.h>

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/util/jaut_CommonUtils.h>

#include <juce_core/juce_core.h>

#include <bitset>



namespace jaut
{
    //==================================================================================================================
    struct ILogSink;
    
    //==================================================================================================================
    /** 
     *  A worker is a separate layer to a jaut::BasicLogger, used to actually transport messages
     *  to the logging sinks.
     *  Different worker implementations might handle log distribution differently.
     *  <br><br>
     *  The worker will act as an advanced buffer that also cares about processing incoming messages so that every
     *  sink gets what it deserves.
     */
    struct JAUT_API ILogWorker
    {
        //==============================================================================================================
        enum class FlushAttemptResult
        {
            /** The worker has not been initialised yet. */
            NotReady,
            
            /** The flush has been successful. */
            Successful,
    
            /** The flush has been unsuccessful. */
            Unsuccessful,
    
            /** The worker can't give back an immediate response as it's currently processing the flush request. */
            Async
        };
        
        //==============================================================================================================
        virtual ~ILogWorker() = default;
    
        //==============================================================================================================
        /**
         *  This sets up the worker.<br>
         *  When this class is used together with a jaut::BasicLogger, you should not call this manually as the logger
         *  will do that for you.
         *  
         *  @param sink        The sink to manage
         *  @param flushPolicy The flush behaviour settings
         */
        virtual void setup(AbstractLogger &logger, const FlushPolicy::Settings &flushPolicy) = 0;
        
        /** If anything needs to be done before shutting down the sink, you can do it with the finalise call. */
        virtual void finalise() {};
        
        //==============================================================================================================
        /**
         *  Enqueues a message into the log buffer if possible.
         *  
         *  @param message The message to enqueue
         *  @return True if the message was enqueued, false if the buffer was full
         */
        virtual bool enqueue(LogMessage message) = 0;
        
        //==============================================================================================================
        /** 
         *  Whether the log buffer is empty.
         *  @return True if the buffer is empty
         */
        JAUT_NODISCARD
        virtual bool isEmpty() const = 0;
    
        /** 
         *  Whether the log buffer is full.
         *  @return True if the buffer is full
         */
        JAUT_NODISCARD
        virtual bool isFull() const = 0;
    
        /** 
         *  The number of messages inside the buffer.
         *  @return The number of messages
         */
        JAUT_NODISCARD
        virtual int size() const = 0;
    
        /** 
         *  The full capacity of the buffer.
         *  @return The capacity
         */
        JAUT_NODISCARD
        virtual int capacity() const = 0;
        
        //==============================================================================================================
        /**
         *  Forces a flush of the buffer to all sinks.
         *  @return True if the worker is able to flush, false if it hasn't been initialised yet
         */
        virtual bool flush() = 0;
        
        /**
         *  Tries to flush the buffer to all sinks if one of the given flushing policies was satisfied.
         *  
         *  @param lastMessage The last message that was enqueued
         *  @param sink        The sink
         *  @return The flush result
         */
        virtual FlushAttemptResult tryFlush(const LogMessage &lastMessage) = 0;
    };
}
