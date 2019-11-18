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
 * File: guidspunitmanager.h
 * Time: 2, July 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/dspgui.h>
#include <jaut/propertyattribute.h>

namespace jaut
{
class IComponentDrawing;
class DspUnitManager;
class PropertyAttribute;

class JAUT_API GuiDspUnitManager final : public DspGui, private Timer
{
public:
    using t_slider_attc = juce::AudioProcessorValueTreeState::SliderAttachment;
    using p_drawing_ins = std::unique_ptr<IComponentDrawing>;

    //=================================================================================================================
    GuiDspUnitManager(DspUnitManager &processor, AudioProcessorValueTreeState &vts) noexcept;

    ~GuiDspUnitManager() override;

    //=================================================================================================================
    void resized() override;

    //=================================================================================================================
    void displayGui(int index) noexcept;
    void setUnit(int index, bool display = true) noexcept;
    const bool isEmptyChildEditor(DspGui &editor) const noexcept;
    void setEmptyDrawingInstance(p_drawing_ins drawingInstance);
    const int getActiveIndex() const noexcept;
    const int getDisplayIndex() const noexcept;
    void reset() noexcept;
    const int getNumGuis() const noexcept;

private:
    DspUnitManager &processor;
    p_drawing_ins emptySurfaceInstance;
    PropertyAttribute selectedIndex;
    OwnedArray<DspGui> editors;
    int displayIndex;
    int activeIndex;

    void timerCallback();

    JUCE_DECLARE_NON_COPYABLE(GuiDspUnitManager)
};
}