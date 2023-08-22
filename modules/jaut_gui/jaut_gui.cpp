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
    
    Copyright (c) 2022 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   jaut_gui.cpp
    @date   18, June 2020
    
    ===============================================================
 */

#include <jaut_gui/jaut_gui.h>



/*
#include <jaut_gui/component/component.cpp>
#include <jaut_gui/mouse/mouse.cpp>
#include <jaut_gui/util/util.cpp>

#include <jaut_gui/text/internal/jaut_format_attributes.h>
#include <jaut_gui/text/internal/jaut_format_layout.h>
#include <jaut_gui/text/internal/jaut_format_attributes.cpp>
#include <jaut_gui/text/internal/jaut_format_layout.cpp>
#include <jaut_gui/text/text.cpp>
*/

// Component
#include <jaut_gui/component/jaut_ContentPane.cpp>
#include <jaut_gui/component/jaut_DragAndDropWindow.cpp>
#include <jaut_gui/component/jaut_ExtraComponent.cpp>
#include <jaut_gui/component/jaut_MultiPagePanel.cpp>
#include <jaut_gui/component/jaut_SplitPanel.cpp>

// LookAndFeel
#include <jaut_gui/lookandfeel/jaut_LookAndFeel.cpp>



namespace jaut
{
    //==================================================================================================================
    LookAndFeel& getDefaultLaf() noexcept
    {
        static LookAndFeel laf;
        return laf;
    }
}
