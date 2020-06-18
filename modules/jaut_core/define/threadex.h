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

#if (JAUT_STRICT_THREAD_EXCLUSION && JUCE_DEBUG) || DOXYGEN
  
    /**
     *  This will make sure that certain functions, specified to only be called on the message-thread can only
     *  be called on the message thread.
     *
     *  This will have no effects on a release builds.
     */
#   define JAUT_ENSURE_MESSAGE_THREAD() JUCE_BLOCK_WITH_FORCED_SEMICOLON \
                                        ( \
                                            if (jaut::internal::ThreadexIsEnabled) \
                                            { \
                                                jassert(juce::MessageManager::getInstance()->isThisTheMessageThread()); \
                                            } \
                                        )

    /** Disables the Threadex until it is turned back on with JAUT_ENABLED_THREADEX. */
#   define JAUT_THREADEX_DISABLE() jaut::internal::ThreadexIsEnabled = false
  
    /** Re-enables the Threadex after it was turned off. */
#   define JAUT_THREADEX_ENABLE()  jaut::internal::ThreadexIsEnabled = true

namespace jaut::internal { extern bool ThreadexIsEnabled; }

#else
#   define JAUT_ENSURE_MESSAGE_THREAD()  do {} while(false)
#   define JAUT_THREADEX_DISABLE()       do {} while(false)
#   define JAUT_THREADEX_ENABLE()        do {} while(false)
#endif

namespace jaut
{
/**
 *  Temporarily disables thread exclusion for the current scope if thread exclusion is enabled and
 *  the current build is a debug build, else this will do nothing.
 *  This should always be preferred over the clumsy macro definitions.
 */
class JAUT_API ThreadexSwitch
{
    ThreadexSwitch() noexcept { JAUT_THREADEX_DISABLE(); }
    ~ThreadexSwitch() { JAUT_THREADEX_ENABLE();  }
};
}
