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

    ID:                 jaut_core
    vendor:             ElandaOfficial
    version:            0.1.0
    name:               JAUT Core Library
    description:        Core functionality of jaut.
    website:            https://github.com/elandasunshine/jaut
    license:            AGPL v3
    minimumCppStandard: 17
    dependencies:       juce_core juce_events

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

// Dependencies
#include <juce_events/juce_events.h>

// Options
//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
/** Config: JAUT_STRICT_MODE
    If strict mode is enabled, the jaut library will assert a lot more often to specific scenarios where
    this is advisable.
 */
#ifndef JAUT_STRICT_MODE
  #define JAUT_STRICT_MODE 1
#endif

#if JAUT_STRICT_MODE
  /** Config: JAUT_STRICT_TEMPLATES
      If this is enabled, certain jaut classes needing objects from interfaces will make sure custom
      implementations will need to inherit from these interfaces to compile.
      Only enabled if JAUT_STRICT_MODE is enabled.
   */
  #ifndef JAUT_STRICT_TEMPLATES
    #define JAUT_STRICT_TEMPLATES 1
  #endif

  /** Config: JAUT_STRICT_THREAD_EXCLUSION
      If this is enabled, the library will assert if message-thread code was not executed from
      the message-thread.
      Only enabled if JAUT_STRICT_MODE is enabled.
   */
  #ifndef JAUT_STRICT_THREAD_EXCLUSION
    #define JAUT_STRICT_THREAD_EXCLUSION 1
  #endif
#endif

/** Config: JAUT_PREFER_JUCE_LISTENERS_OVER_EVENTS
    If this is enabled, all jaut classes will use JUCE listeners instead of jaut Events.
 */
#ifndef JAUT_PREFER_JUCE_LISTENERS_OVER_EVENTS
  #define JAUT_PREFER_JUCE_LISTENERS_OVER_EVENTS 0
#endif

// Module headers
//======================================================================================================================
//======================================================================================================================
//======================================================================================================================
// Library defines
#include <jaut_core/define/assertdef.h>
#include <jaut_core/define/jautdef.h>
#include <jaut_core/define/threadex.h>
#include <jaut_core/define/utils.h>

//======================================================================================================================
// Preprocessor general
#include <jaut_core/preprocessor/upp_assert.h>
#include <jaut_core/preprocessor/upp_misc.h>

// Define
#if !DOXYGEN
  #include <jaut_core/preprocessor/define/arguments.def>
  #include <jaut_core/preprocessor/define/collection.def>
  #include <jaut_core/preprocessor/define/conditional.def>
#endif

// Arguments
#include <jaut_core/preprocessor/arguments/upp_args.h>

// Collection
// #include <jaut_core/preprocessor/collection/upp_group.h> // experimental

// Conditional
#include <jaut_core/preprocessor/conditional/upp_cond.h>
#include <jaut_core/preprocessor/conditional/upp_if.h>

//======================================================================================================================
// Interfaces
#include <jaut_core/interfaces/iexpandable.h>
#include <jaut_core/interfaces/iexpansionunit.h>

//======================================================================================================================
// Misc utils
#include <jaut_core/util/exception.h>
#include <jaut_core/util/mathutil.h>
#include <jaut_core/util/typetraits.h>
#include <jaut_core/util/operationresult.h>
#include <jaut_core/util/objectproxy.h>
#include <jaut_core/util/version.h>

//======================================================================================================================
// Signaling handlers
#include <jaut_core/signal/event/event.h>

//======================================================================================================================
// Math utils
#include <jaut_core/math/numeric.h>

//======================================================================================================================
// Text utils
#include <jaut_core/text/alphabetiterator.h>
#include <jaut_core/text/expansion/expandables.h>
#include <jaut_core/text/expansion/expansionunit.h>
