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

    ID:                 jaut_provider
    vendor:             ElandaOfficial
    version:            0.1.0
    name:               JAUT Data & Storage Classes
    description:        Defines classes for advanced data management like configurations and folder management.
    website:            https://github.com/elandasunshine/jaut
    license:            AGPL v3
    minimumCppStandard: C++17
    dependencies:       jaut_core, juce_core, juce_events, juce_graphics

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

// Dependencies
#include <jaut_core/jaut_core.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>

// Interfaces
#include <jaut_provider/interfaces/iconfigparser.h>
#include <jaut_provider/interfaces/imetadata.h>
#include <jaut_provider/interfaces/imetareader.h>
#include <jaut_provider/interfaces/ithemedefinition.h>

// Config
#include <jaut_provider/config/config.h>
#include <jaut_provider/config/configparser.h>

// File
#include <jaut_provider/file/appdata.h>
