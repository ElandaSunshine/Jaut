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
    @file   jaut_IMessageBuffer.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/util/jaut_CommonUtils.h>
#include <jaut_message/thread/message/jaut_IMessage.h>



namespace jaut
{
    /** 
     *  The IMessageBuffer interface declares all needed virtual functions to craft your own message buffer.
     *  @tparam T The type of the message to send
     */
    template<class T = std::unique_ptr<IMessage>>
    struct JAUT_API IMessageBuffer
    {
        //==============================================================================================================
        /** The message type. */
        using Message = T;
    
        //==============================================================================================================
        virtual ~IMessageBuffer() = default;
        
        //==============================================================================================================
        /**
         *  Pushes a new Message onto the buffer.
         *  
         *  @param message The message to push
         *  @return The position the message was enqueued in, -1 if the buffer was full
         */
        virtual int push(Message message) = 0;
        
        /**
         *  Pops a previously pushed message from the buffer and returns it.
         *  @return The popped message
         */
        JAUT_NODISCARD
        virtual Message pop() = 0;
        
        //==============================================================================================================
        /**
         *  Determines whether the buffer is empty.
         *  @return True if empty, false if not
         */
        JAUT_NODISCARD
        virtual bool isEmpty() const = 0;
        
        /**
         *  Determines whether the buffer is full.
         *  @return True if full, false if not
         */
        JAUT_NODISCARD
        virtual bool isFull() const = 0;
        
        //==============================================================================================================
        /**
         *  Gets the current amount of elements in the buffer.
         *  @return The number of pushed messages
         */
        JAUT_NODISCARD
        virtual int size() const = 0;
        
        /**
         *  Gets the absolute amount of elements this buffer can contain.
         *  @return The capacity of the buffer
         */
        JAUT_NODISCARD
        virtual int capacity() const = 0;
    };
}
