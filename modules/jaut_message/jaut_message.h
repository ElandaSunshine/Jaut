/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.

    Copyright (c) 2019 ElandaSunshine
    ===============================================================

    @author Elanda (elanda@elandasunshine.xyz)
    @file   jaut_message.h
    @date   29, February 2020

    ===============================================================
    BEGIN_JUCE_MODULE_DECLARATION

    ID:                 jaut_message
    vendor:             ElandaOfficial
    version:            0.1.0
    name:               JAUT Message & Threading Utilities
    description:        Tools for simple message passing management.
    website:            https://github.com/elandasunshine/jaut
    license:            AGPL v3
    minimumCppStandard: C++17
    dependencies:       juce_core juce_events

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/jaut_message/interfaces/imessage.h>
#include <jaut/jaut_message/interfaces/imessagebuffer.h>
#include <jaut/jaut_message/buffer/atomicringbuffer.h>
#include <jaut/jaut_message/manager/messagehandler.h>