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
    @file   jaut_provider.h
    @date   29, February 2020

    ===============================================================
    BEGIN_JUCE_MODULE_DECLARATION

    ID:                 jaut_provider
    vendor:             ElandaSunshine
    version:            1.0.0
    name:               JAUT Data & Storage Classes
    description:        Defines classes for advanced data management like configurations and folder management.
    website:            https://github.com/elandasunshine/jaut
    license:            GPL v3
    minimumCppStandard: 17
    dependencies:       jaut_core juce_events juce_graphics

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

// Depends
#include <jaut_provider/jaut_provider_define.h>

// Main
#include <jaut_core/jaut_core.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>

// Config 
#include <jaut_provider/config/jaut_Config.h>
#include <jaut_provider/config/jaut_ConfigException.h>
#include <jaut_provider/config/jaut_IConfigParser.h>
#include <jaut_provider/config/parser/jaut_XmlParser.h>
#include <jaut_provider/config/parser/jaut_JsonParser.h>
#include <jaut_provider/config/parser/jaut_YamlParser.h>
