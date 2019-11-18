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
 * File: dynamicgui.h
 * Time: 2, July 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/dspgui.h>

namespace jaut
{
class IComponentDrawing;
class JAUT_API DynamicGui : public DspGui
{
public:
    DynamicGui(DspUnit &processor, IComponentDrawing *drawingInstance,
               bool delayedInitialization = false) noexcept;
    ~DynamicGui() override {}

    //=================================================================================================================
    void initializeComponent() noexcept;

    //=================================================================================================================
    virtual void paint(Graphics &g) override;
    virtual void resized() override;

private:
    bool hasBeenInitialized;
    std::unique_ptr<IComponentDrawing> surfaceInstance;

    JUCE_DECLARE_NON_COPYABLE(DynamicGui)
};
}