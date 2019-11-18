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
 * File: dynamicgui.cpp
 * Time: 2, July 2019
 *
 * ===============================================================
 */

#include <jaut/dynamicgui.h>
#include <jaut/icomponentdrawing.h>

namespace jaut
{
DynamicGui::DynamicGui(DspUnit &processor, IComponentDrawing *drawingInstance, bool delayedInitialization) noexcept
    : DspGui(processor),
      surfaceInstance(drawingInstance),
      hasBeenInitialized(false)
{
    if (drawingInstance && !delayedInitialization)
    {
        drawingInstance->initializeComponents(this);
        hasBeenInitialized = true;
    }
}

void DynamicGui::initializeComponent() noexcept
{
    if (surfaceInstance && !hasBeenInitialized)
    {
        surfaceInstance->initializeComponents(this);
        hasBeenInitialized = true;
    }
}

//=====================================================================================================================
void DynamicGui::paint(Graphics &g)
{
    if (surfaceInstance)
    {
        surfaceInstance->paint(g);
    }
}

void DynamicGui::resized()
{
    if (surfaceInstance)
    {
        surfaceInstance->resized();
    }
}
}