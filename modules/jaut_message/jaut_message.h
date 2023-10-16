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
    @file   jaut_message.h
    @date   29, February 2020

    ===============================================================
    BEGIN_JUCE_MODULE_DECLARATION

    ID:                 jaut_message
    vendor:             ElandaSunshine
    version:            1.0.0
    name:               JAUT Message & Threading Utilities
    description:        Tools for simple message passing management.
    website:            https://github.com/elandasunshine/jaut
    license:            GPL v3
    minimumCppStandard: 17
    dependencies:       jaut_core juce_events

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once



// Dependencies
#include <jaut_core/jaut_core.h>
#include <juce_events/juce_events.h>

// Thread
#include <jaut_message/thread/jaut_MessageDirection.h>
#include <jaut_message/thread/jaut_MessageHandler.h>
#include <jaut_message/thread/buffer/jaut_AtomicRingBuffer.h>
#include <jaut_message/thread/buffer/jaut_SimpleRingBuffer.h>
#include <jaut_message/thread/exception/jaut_QueueSpaceExceededException.h>
#include <jaut_message/thread/message/inbuilt/jaut_MessageCallback.h>
#include <jaut_message/thread/message/jaut_IMessage.h>
#include <jaut_message/thread/message/jaut_IMessageBuffer.h>
