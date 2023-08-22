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
    @file   jaut_core.h
    @date   29, February 2020

    ===============================================================
    BEGIN_JUCE_MODULE_DECLARATION

    ID:                 jaut_core
    vendor:             ElandaSunshine
    version:            0.1.0
    name:               JAUT Core Library
    description:        Core functionality of jaut.
    website:            https://github.com/elandasunshine/jaut
    license:            GPL v3
    minimumCppStandard: 17
    dependencies:       juce_events

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once



//======================================================================================================================
#include <juce_events/juce_events.h>

//======================================================================================================================
// Define
#include <jaut_core/define/jaut_AssertDef.h>
#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/define/jaut_DefUtils.h>

// Math
#include <jaut_core/math/jaut_MathUtil.h>
#include <jaut_core/math/jaut_Numeric.h>
#include <jaut_core/math/jaut_SafeFloat.h>
#include <jaut_core/math/jaut_SafeInteger.h>

// Preprocessor
#include <jaut_core/preprocessor/jaut_UppAssert.h>
#include <jaut_core/preprocessor/jaut_UppMisc.h>
#include <jaut_core/preprocessor/arguments/jaut_UppArgs.h>
#include <jaut_core/preprocessor/conditional/jaut_UppConditional.h>
#include <jaut_core/preprocessor/conditional/jaut_UppIf.h>
#if !DOXYGEN
#   include <jaut_core/preprocessor/define/arguments.def>
#   include <jaut_core/preprocessor/define/collection.def>
#   include <jaut_core/preprocessor/define/conditional.def>
#endif

// Signal
#include <jaut_core/signal/jaut_RwLockGuard.h>
#include <jaut_core/signal/event/jaut_Event.h>

// Util
#include <jaut_core/util/jaut_CommonUtils.h>
#include <jaut_core/util/jaut_OperationResult.h>
#include <jaut_core/util/jaut_Stringable.h>
#include "jaut_gui/util/jaut_ScopedCursor.h"
#include <jaut_core/util/jaut_TypeContainer.h>
#include <jaut_core/util/jaut_TypeEvaluator.h>
#include <jaut_core/util/jaut_TypeTraits.h>
#include <jaut_core/util/jaut_VarUtil.h>
#include <jaut_core/util/jaut_Version.h>
