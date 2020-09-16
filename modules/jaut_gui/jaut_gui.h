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
namespace DefaultColours
{
    const juce::Colour Colour_Background { 0xff282828 };
    const juce::Colour Colour_Border     { 0xccffffff };
    const juce::Colour Colour_Text       { 0xccffffff };
    
    const juce::Colour Colour_Button     { 0xff282828 };
    const juce::Colour Colour_ButtonText { 0xccffffff };
    const juce::Colour Colour_ButtonDown = Colour_Button.brighter(0.2f);
    const juce::Colour Colour_ButtonOver = Colour_Button.brighter(0.3f);
};

class LookAndFeel_Jaut;
}

#if !DOXYGEN
#   define JAUT_CREATE_LAF() \
           LookAndFeel_Jaut *lookAndFeel { nullptr }; \
           void parentHierarchyChanged() final override; \
           void lookAndFeelChanged() final override;
#endif

// Structure
#include <jaut_gui/structure/sizestructs.h>

// Mouse
#include <jaut_gui/mouse/draganddropcontaineradvanced.h>

// Components
#include <jaut_gui/component/contentpane.h>
#include <jaut_gui/component/splitcontainer.h>
#include <jaut_gui/component/multipagepane.h>
#include <jaut_gui/component/dockingpane.h>

// Includes
#include <jaut_gui/text/charformat.h>
#include <jaut_gui/text/fontformat.h>

// Laf
#include <jaut_gui/component/lookandfeel_jaut.h>

#undef JAUT_CREATE_LAF
