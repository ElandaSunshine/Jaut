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
    @file   jaut_IMessage.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_message/thread/jaut_MessageDirection.h>



namespace jaut
{
    //==================================================================================================================
    struct IMessageHandler;
    
    //==================================================================================================================
    /** Implements a message policies that should be passed on to the policies. */
    struct JAUT_API IMessage
    {
        //==============================================================================================================
        virtual ~IMessage() = default;
    
        //==============================================================================================================
        /**
         *  Processes the message when it arrived on the other side of the policies.
         *  
         *  @param context          The context, or the policies which passed the message around
         *  @param messageDirection The direction the message went
         */
        virtual void handleMessage(IMessageHandler *context, MessageDirection messageDirection) = 0;
    
        //==============================================================================================================
        /** 
         *  Provides a way to instruct the target thread to create a message without actually allocating one.
         *  
         *  On the target thread, you can write the data into the passed on data object and the policies will send it
         *  back to the message thread which will then handle the message.
         *  
         *  Deferred messages sent from the target thread, will be treated like normal messages.
         *  
         *  This method can be override to provide a numerical id for a deferred message.
         *  If this is above or equal 0, it will be treated as a deferred message, everything below is a normal message.
         *  
         *  @return The id of the deferred message, or -1 for normal messages
         */
        virtual int getDeferId() { return -1; }
    };
}
