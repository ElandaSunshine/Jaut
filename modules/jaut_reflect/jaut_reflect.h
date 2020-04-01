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

    ID:                 jaut_reflect
    vendor:             ElandaOfficial
    version:            0.1.0
    name:               JAUT Reflection & Metaprogramming Tools
    description:        Useful tools for metaprogramming and reflection support in C++ and JUCE.
    website:            https://github.com/elandasunshine/jaut
    license:            AGPL v3
    minimumCppStandard: C++17
    dependencies:       jaut_core, juce_core, juce_events

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

#include <jaut_core/jaut_core.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

/** Config: JAUT_THROW_IF_WRONG_TAG_TARGET

    If this is set to true, when a tag was attached to an unsupported scope type, this will throw an exception.
    Otherwise, will merely print a simple warning.
 */
#ifndef JAUT_THROW_IF_WRONG_TAG_TARGET
  #define JAUT_THROW_IF_WRONG_TAG_TARGET 1
#endif

#include <jaut_reflect/refldef.h>

// Tags
#include <jaut_reflect/refltag/refltag.h>
#include <jaut_reflect/refltag/nativetags.h>
#include <jaut_reflect/refltag/tagutil.h>
#include <jaut_reflect/refltag/tagdescriptor.h>

#include <jaut_reflect/class/classtype.h>
