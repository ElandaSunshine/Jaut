/**
    ===============================================================
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

    Copyright (c) 2019 ElandaSunshine
    ===============================================================

    @author Elanda
    @file   jaut_audio.h
    @date   29, February 2020

    ===============================================================
    BEGIN_JUCE_MODULE_DECLARATION

    ID:                 jaut_audio
    vendor:             ElandaOfficial
    version:            1.0.0
    name:               JAUT Audio & Processor Classes
    description:        Various processing and dsp related classes.
    website:            https://github.com/elandasunshine/jaut
    license:            AGPL v3
    minimumCppStandard: 17
    dependencies:       jaut_message juce_audio_processors

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

#include <jaut_message/jaut_message.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <jaut_audio/parameter/attachments.h>
#include <jaut_audio/parameter/virtualparameter.h>

#include <jaut_audio/processor/processspec.h>
#include <jaut_audio/processor/serialisableaudioprocessor.h>
#include <jaut_audio/processor/audioprocessorrack.h>
#include <jaut_audio/processor/audioprocessorset.h>
