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
    @file   jaut_MessageCallback.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/util/jaut_CommonUtils.h>
#include <jaut_message/thread/message/jaut_IMessage.h>
#include <jaut_message/thread/jaut_MessageDirection.h>

#include <functional>



namespace jaut::message
{
    //==================================================================================================================
    class JAUT_API MessageCallback : public IMessage
    {
    public:
        using CallbackHandler = std::function<void(IMessageHandler *context, MessageDirection messageDirection)>;
    
        //==============================================================================================================
        explicit MessageCallback(NonNull<CallbackHandler> handler) noexcept;
        
        //==============================================================================================================
        void handleMessage(IMessageHandler *context, MessageDirection messageDirection) override;
        
    private:
        CallbackHandler handler;
    };
    
    //==================================================================================================================
    // IMPLEMENTATION
    inline MessageCallback::MessageCallback(NonNull<CallbackHandler> parHandler) noexcept
        : handler(std::move(parHandler))
    {}

    //==================================================================================================================
    inline void MessageCallback::handleMessage(jaut::IMessageHandler  *parContext,
                                               jaut::MessageDirection parMessageDirection)
    {
        handler(parContext, parMessageDirection);
    }
}
