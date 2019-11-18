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
 * File: guidspunitmanager.cpp
 * Time: 2, July 2019
 *
 * ===============================================================
 */

#include <jaut/guidspunitmanager.h>
#include <jaut/dspunitmanager.h>
#include <jaut/icomponentdrawing.h>
#include <jaut/propertyattribute.h>

namespace jaut
{
struct EmptyEditor final : public DspGui
{
    IComponentDrawing *surfaceDrawing;

    //=================================================================================================================
    EmptyEditor(DspUnit &processor, IComponentDrawing *drawingInstance)
        : DspGui(processor), surfaceDrawing(drawingInstance)
    {}

    //=================================================================================================================
    void paint(Graphics &g) override
    {
        if (surfaceDrawing)
        {
            surfaceDrawing->paint(g);
        }
    }

    void resized() override
    {
        if (surfaceDrawing)
        {
            surfaceDrawing->resized();
        }
    }

    //=================================================================================================================
    void setDrawingInstance(IComponentDrawing *drawing) noexcept
    {
        if (surfaceDrawing != drawing)
        {
            surfaceDrawing = drawing;

            removeAllChildren();
            drawing->initializeComponents(this);
            repaint();
        }
    }

    JUCE_DECLARE_NON_COPYABLE(EmptyEditor)
};

GuiDspUnitManager::GuiDspUnitManager(DspUnitManager &processor, AudioProcessorValueTreeState &vts) noexcept
    : DspGui(processor), processor(processor),
      emptySurfaceInstance(nullptr),
      selectedIndex(processor.properties, "SelectedIndex"),
      displayIndex(0), activeIndex(0)
{
    activeIndex  = static_cast<var>(selectedIndex);
    displayIndex = static_cast<var>(selectedIndex);

    reset();
    startTimer(100);

    displayGui(activeIndex);
}

GuiDspUnitManager::~GuiDspUnitManager()
{}

//=====================================================================================================================
void GuiDspUnitManager::resized()
{
    for (auto *editor : editors)
    {
        editor->setBounds(getLocalBounds());
    }
}

//=====================================================================================================================
void GuiDspUnitManager::displayGui(int index) noexcept
{
    bool flag = processor.getNumProcessors() == getNumGuis();
    jassert(flag);

    if (flag)
    {
        reset();
    }

    if (displayIndex != index && index >= 0 && index < processor.getNumProcessors())
    {
        auto *curgui = editors[displayIndex];
        auto *nexgui = editors.getUnchecked(index);

        if (curgui)
        {
            curgui->setVisible(false);
        }

        nexgui->setVisible(true);
    }
}

void GuiDspUnitManager::setUnit(int index, bool display) noexcept
{
    bool flag  = processor.getNumProcessors() == getNumGuis();
    bool flag2 = display && displayIndex != index;
    jassert(flag);

    if (flag)
    {
        reset();
    }

    if (activeIndex != index && index >= 0 && index < processor.getNumProcessors())
    {
        selectedIndex = index;
        auto *curgui = editors[displayIndex];
        auto *nexgui = editors.getUnchecked(index);

        if (curgui)
        {
            if (flag2)
            {
                curgui->setVisible(false);
            }

            curgui->setEnabled(false);
        }

        if (flag2)
        {
            nexgui->setVisible(true);
        }

        nexgui->setEnabled(true);
    }
}

const bool GuiDspUnitManager::isEmptyChildEditor(DspGui &editor) const noexcept
{
    return editors.contains(&editor) && dynamic_cast<EmptyEditor*>(&editor);
}

void GuiDspUnitManager::setEmptyDrawingInstance(p_drawing_ins drawingInstance)
{
    if (emptySurfaceInstance.get() != drawingInstance.get())
    {
        for (auto *editor : editors)
        {
            if (auto *emptyEditor = dynamic_cast<EmptyEditor*>(editor))
            {
                emptyEditor->setDrawingInstance(emptySurfaceInstance.get());
            }
        }
    }
}

const int GuiDspUnitManager::getActiveIndex() const noexcept
{
    return activeIndex;
}

const int GuiDspUnitManager::getDisplayIndex() const noexcept
{
    return displayIndex;
}

void GuiDspUnitManager::reset() noexcept
{
    removeAllChildren();
    editors.clear();

    for (auto *proc : processor.processors)
    {
        DspGui *editor = proc->createGui();

        if (!editor)
        {
            editor = new EmptyEditor(*proc, emptySurfaceInstance.get());
        }

        editor->setEnabled(false);
        addChildComponent(editor);
        editors.add(editor);
    }
}

const int GuiDspUnitManager::getNumGuis() const noexcept
{
    return editors.size();
}

//=====================================================================================================================
void GuiDspUnitManager::timerCallback()
{
    const int newvalue = static_cast<var>(selectedIndex);

    if (activeIndex != newvalue)
    {
        setUnit(newvalue, false);
    }
}
}