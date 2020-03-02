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
    minimumCppStandard: C++17
    dependencies:       juce_core, juce_data_structures, juce_audio_processors, juce_events, jaut_message

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>

#include <jaut/jaut_audio/processor/serializableaudioprocessor.h>
#include <jaut/jaut_audio/processor/audioprocessoreffect.h>
#include <jaut/jaut_audio/processor/audioprocessorrack.h>
