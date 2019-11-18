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
 * File: dspunitmanager.cpp
 * Time: 25, June 2019
 *
 * ===============================================================
 */

#include <jaut/dspunitmanager.h>
#include <jaut/guidspunitmanager.h>

namespace jaut
{
class PlaceholderUnit final : public DspUnit
{
public:
    PlaceholderUnit(DspUnit &proc)
        : DspUnit(proc)
    {}

    const String getName() const override
    {
        return "Digital Signal Processing Placeholder";
    }

    bool hasEditor() const override
    {
        return false;
    }

    void process(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer) override {}
    void beginPlayback(double sampleRate, int maxBlockSamples) override {}
    void finishPlayback() override {}
    void readData(const ValueTree data) override {}
    void writeData(ValueTree data) const override {}

    DspGui *getGuiType() override
    {
        return nullptr;
    }

private:
    JUCE_DECLARE_NON_COPYABLE(PlaceholderUnit)
};

DspUnitManager::DspUnitManager(AudioProcessor &parentProcessor, AudioProcessorValueTreeState &vts,
                               UndoManager *undoManager, int initialCapacity, bool unitsAreOwned) noexcept
    : DspUnit(parentProcessor, &vts, undoManager),
      storage(initialCapacity), isOwned(unitsAreOwned)
{
    properties.createProperty("SelectedIndex", 0);
    properties.addListener(this);
    addProcessorType(new PlaceholderUnit(*this));
}

DspUnitManager::~DspUnitManager()
{
    if (getNumProcessors() > 0)
    {
        if (auto *pp = dynamic_cast<PlaceholderUnit*>(processors.getUnchecked(0)))
        {
            delete pp;
        }
        else if (isOwned)
        {
            for (auto *proc : processors)
            {
                delete proc;
                proc = nullptr;
            }
        }
    }
}

//=====================================================================================================================
void DspUnitManager::addProcessorType(DspUnit *processor, bool notifyGui) noexcept
{
    JAUT_ENSURE_AUDIO_THREAD();

    if (processor)
    {
        if (getNumProcessors() > 0 && dynamic_cast<PlaceholderUnit*>(processors.getUnchecked(0)))
        {
            Array<DspUnit*> tempArray;

            if (storage > 0)
            {
                tempArray.ensureStorageAllocated(storage);
            }

            tempArray.add(processor);
            processors.swapWith(tempArray);
            delete tempArray.removeAndReturn(0);

            if (notifyGui)
            {
                if (MessageManager::getInstance()->isThisTheMessageThread())
                {
                    cancelPendingUpdate();
                    handleAsyncUpdate();
                }
                else
                {
                    triggerAsyncUpdate();
                }
            }

            return;
        }

        for (auto *proc : processors)
        {
            if (processor->getName() == proc->getName())
            {
                return;
            }
        }

        processors.add(processor);

        if (notifyGui)
        {
            if (MessageManager::getInstance()->isThisTheMessageThread())
            {
                cancelPendingUpdate();
                handleAsyncUpdate();
            }
            else
            {
                triggerAsyncUpdate();
            }
        }
    }
}

void DspUnitManager::setCurrentProcessor(int processorIndex)
{
    JAUT_ENSURE_AUDIO_THREAD();

    if (processorIndex >= 0 && processorIndex < getNumProcessors() && selectedIndex != processorIndex)
    {
        properties.setProperty("SelectedIndex", processorIndex);
        selectedIndex = processorIndex;
    }
}

DspUnit *DspUnitManager::getCurrentProcessor()
{
    return processors.getUnchecked(selectedIndex);
}

//=================================================================================================================
const int DspUnitManager::getNumProcessors() const noexcept
{
    return processors.size();
}

const String DspUnitManager::getName() const
{
    return "Digital Signal Processing Unit Manager";
}

bool DspUnitManager::hasEditor() const
{
    return true;
}

const bool DspUnitManager::doesOwnUnits() const noexcept
{
    return isOwned;
}

//=====================================================================================================================
void DspUnitManager::process(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer)
{
    getCurrentProcessor()->processBlock(buffer, midiBuffer);
}

void DspUnitManager::process(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer)
{
    getCurrentProcessor()->processBlock(buffer, midiBuffer);
}

void DspUnitManager::beginPlayback(double sampleRate, int maxBlockSamples)
{
    for (auto *proc : processors)
    {
        proc->prepareToPlay(sampleRate, maxBlockSamples);
    }
}

void DspUnitManager::finishPlayback()
{
    for (auto *proc : processors)
    {
        proc->releaseResources();
    }
}

void DspUnitManager::reset()
{
    for (auto *proc : processors)
    {
        proc->reset();
    }
}

//=====================================================================================================================
void DspUnitManager::readData(const ValueTree data)
{
    if (!data.isValid())
    {
        return;
    }

    properties.readFrom(data.getChildWithName("Properties"));
    ValueTree units = data.getChildWithName("Units");

    if (units.isValid())
    {
        for(auto unit : units)
        {
            int index = unit.getProperty("Index");

            if(index < processors.size() && index >= 0)
            {
                DspUnit *proc = processors.getUnchecked(index);

                if(proc->getName() == unit.getProperty("Name").toString())
                {
                    proc->readData(unit);
                }
            }
        }
    }
}

void DspUnitManager::writeData(ValueTree data) const
{
    if (!data.isValid())
    {
        return;
    }

    data.getOrCreateChildWithName("Properties", 0).copyPropertiesAndChildrenFrom(properties.writeTo("Properties"), 0);
    ValueTree units = data.getOrCreateChildWithName("Units", nullptr);
    units.removeAllChildren(nullptr);

    for (int i = 0; i < processors.size(); ++i)
    {
        DspUnit *proc = processors.getUnchecked(i);

        ValueTree unit = ValueTree("Unit");
        unit.setProperty("Name", proc->getName(), nullptr);
        unit.setProperty("Index", i, nullptr);
        proc->writeData(unit);
        units.appendChild(unit, nullptr);
    }
}

//=====================================================================================================================
void DspUnitManager::onValueChanged(const String &name, var oldValue, var newValue)
{
    selectedIndex = newValue;
}

DspGui *DspUnitManager::getGuiType()
{
    return new GuiDspUnitManager(*this, *parameters);
}

void DspUnitManager::handleAsyncUpdate()
{
    if (auto *gui = dynamic_cast<GuiDspUnitManager*>(activeGui))
    {
        gui->reset();
    }
}
}
