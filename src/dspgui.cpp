/**
 * ===============================================================
 * This file is part of the Esac-Jaut library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2019 ElandaSunshine
 * ===============================================================
 *
 * Author: Elanda
 * File: dspgui.cpp
 * Time: 11, July 2019
 *
 * ===============================================================
 */

#include <jaut/dspgui.h>
#include <jaut/dspunit.h>
#include <jaut/propertyattribute.h>

namespace jaut
{
//=====================================================================================================================
DspGui::DspGui(DspUnit &processor) noexcept
    : AudioProcessorEditor(processor),
      processor(processor),
      vts(processor.parameters),
      properties(processor.properties)
{}

}