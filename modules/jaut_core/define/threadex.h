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
    @file   threadex.h
    @date   19, March 2020

    ===============================================================
 */

#pragma once

#if JAUT_STRICT_THREAD_EXCLUSION
  
  /**
   *  This will make sure that certain functions, specified to only be called on the message-thread can only
   *  be called on the message thread.
   *  This will have no effect in release builds.
   */
  #define JAUT_ENSURE_MESSAGE_THREAD JAUT_DEBUGGING(JUCE_BLOCK_WITH_FORCED_SEMICOLON  \
                                     ( \
                                         if (jaut::internal::ThreadexIsEnabled) \
                                         { \
                                             jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());\
                                         } \
                                     ))
  
  /**
   *  Disables the Threadex until it is turned back on with JAUT_ENABLED_THREADEX.
   */
  #define JAUT_DISABLE_THREADEX jaut::internal::ThreadexIsEnabled = false
  
  /**
   *  Re-enables the Threadex after it was turned off.
   */
  #define JAUT_ENABLE_THREADEX  jaut::internal::ThreadexIsEnabled = true

namespace jaut
{
namespace internal
{
    extern bool ThreadexIsEnabled;
}
}

#else
  #define JAUT_DISABLE_THREADEX
  #define JAUT_ENSURE_MESSAGE_THREAD
  #define JAUT_ENABLE_THREADEX
#endif

namespace jaut
{
/**
 *  Temporarily disables thread exclusion for the current scope if thread exclusion is enabled and
 *  the current build is a debug build, else this will do nothing.
 *  This should always be preferred over the clumsy macro definitions.
 */
class JAUT_API ThreadexSwitch final
{
    ThreadexSwitch() noexcept { JAUT_DISABLE_THREADEX; }
    ~ThreadexSwitch() { JAUT_ENABLE_THREADEX;  }
};
}
