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
    @file   jaut_gui.h
    @date   18, June 2020
    
    ===============================================================
    BEGIN_JUCE_MODULE_DECLARATION

    ID:                 jaut_gui
    vendor:             ElandaOfficial
    version:            0.1.0
    name:               JAUT Gui Tools
    description:        Gui related utils.
    website:            https://github.com/elandasunshine/jaut
    license:            AGPL v3
    minimumCppStandard: 17
    dependencies:       jaut_core juce_gui_extra

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */
 
#pragma once

// Dependencies
#include <jaut_core/jaut_core.h>
#include <juce_gui_extra/juce_gui_extra.h>

namespace jaut
{
struct JAUT_API DefaultColours
{
    inline static const juce::Colour Colour_Background = juce::Colours::lightgrey;
    inline static const juce::Colour Colour_Foreground = juce::Colours::grey;
    inline static const juce::Colour Colour_Border     = juce::Colours::darkgrey;
    inline static const juce::Colour Colour_Text       = juce::Colours::black;
};

class LookAndFeel_Jaut;
}

#if !DOXYGEN
#   define JAUT_CREATE_LAF() \
    LookAndFeel_Jaut *lookAndFeel { nullptr }; \
    void parentHierarchyChanged() override; \
    void lookAndFeelChanged() override;
#endif

// Components
#include <jaut_gui/component/contentpane.h>
#include <jaut_gui/component/splitcontainer.h>
#include <jaut_gui/component/lookandfeel_jaut.h>

// Includes
#include <jaut_gui/text/fontformat.h>

// Structure
#include <jaut_gui/structure/sizestructs.h>

#undef JAUT_CREATE_LAF
