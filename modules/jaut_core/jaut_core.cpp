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
    @date   13, March 2020

    ===============================================================
 */

#include <jaut_core/jaut_core.h>

using namespace juce;

#include <jaut_core/text/alphabetiterator.cpp>
#include <jaut_core/text/expansion/expandables.cpp>
#include <jaut_core/text/expansion/expansionunit.cpp>

#if JAUT_STRICT_THREAD_EXCLUSION
namespace jaut::internal { bool ThreadexIsEnabled = true; }
#endif
