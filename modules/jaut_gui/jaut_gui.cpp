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
    @file   jaut_gui.cpp
    @date   18, June 2020
    
    ===============================================================
 */

#include <jaut_gui/jaut_gui.h>
#if !DOXYGEN
#   define JAUT_IMPL_LAF(CLASS) \
    void CLASS ::parentHierarchyChanged() { lookAndFeelChanged(); }\
    void CLASS ::lookAndFeelChanged() \
    { \
        if (lookAndFeel = dynamic_cast<LookAndFeel_Jaut*>(&getLookAndFeel()); !lookAndFeel) \
        { lookAndFeel = &LookAndFeel_Jaut::getDefaultLaf(); } \
    }
#endif

#include <jaut_gui/text/internal/jaut_format_attributes.h>
#include <jaut_gui/text/internal/jaut_format_layout.h>
#include <jaut_gui/text/internal/jaut_format_attributes.cpp>
#include <jaut_gui/text/internal/jaut_format_layout.cpp>

#include <jaut_gui/text/fontformat.cpp>
#include <jaut_gui/component/component.cpp>

#undef JAUT_IMPL_LAF
