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
    @file   dspunitmanager.cpp
    @date   25, June 2019
    
    ===============================================================
 */

#include <jaut/dspunitmanager.h>
#include <jaut/guidspunitmanager.h>

namespace jaut
{

namespace
{
inline void notify(AsyncUpdater &updated, bool shouldNotify)
{
    if(!shouldNotify)
    {
        return;
    }

    if(MessageManager::getInstance()->isThisTheMessageThread())
    {
        updated.cancelPendingUpdate();
        updated.handleAsyncUpdate();
    }
    else
    {
        updated.triggerAsyncUpdate();
    }
}
}



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
      selectedIndex(0), storage(initialCapacity), size(0), isOwned(unitsAreOwned)
{
    properties.createProperty("SelectedIndex", 0);
    properties.addListener(this);
    addProcessorType(new PlaceholderUnit(*this), false);
    size = 0;
}

DspUnitManager::~DspUnitManager()
{
    if (processors.size() > 0)
    {
        if (auto *pp = dynamic_cast<PlaceholderUnit*>(processors.getUnchecked(0)))
        {
            delete pp;
        }
        else if (isOwned)
        {
            for (int i = 0; i < processors.size(); ++i)
            {
                delete processors.getUnchecked(i);
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
        if (processors.size() > 0 && dynamic_cast<PlaceholderUnit*>(processors.getUnchecked(0)))
        {
            Array<DspUnit*> temp_array;

            if (storage > 0)
            {
                temp_array.ensureStorageAllocated(storage);
            }

            temp_array.add(processor);
            processors.swapWith(temp_array);
            delete temp_array.removeAndReturn(0);
            ++size;
            notify(*this, notifyGui);

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
        ++size;
        notify(*this, notifyGui);
    }
}

DspUnit *DspUnitManager::getProcessor(int index) const noexcept
{
    return index >= 0 && index < size ? processors.getUnchecked(index) : nullptr;
}

DspUnit *DspUnitManager::getCurrentProcessor()
{
    return selectedIndex >= 0 && selectedIndex < size ? processors.getUnchecked(selectedIndex) : nullptr;
}

void DspUnitManager::setCurrentProcessor(int processorIndex)
{
    JAUT_ENSURE_AUDIO_THREAD();

    if (processorIndex >= 0 && processorIndex < size && selectedIndex != processorIndex)
    {
        properties.setProperty("SelectedIndex", processorIndex);
        selectedIndex = processorIndex;
    }
}

//=================================================================================================================
const int DspUnitManager::getNumProcessors() const noexcept
{
    return size;
}

const String DspUnitManager::getName() const
{
    return "DspUnitManager";
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
    processors.getUnchecked(selectedIndex)->process(buffer, midiBuffer);
}

void DspUnitManager::process(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer)
{
    processors.getUnchecked(selectedIndex)->process(buffer, midiBuffer);
}

void DspUnitManager::beginPlayback(double sampleRate, int maxBlockSamples)
{
    for (auto *proc : processors)
    {
        proc->beginPlayback(sampleRate, maxBlockSamples);
    }
}

void DspUnitManager::finishPlayback()
{
    for (auto *proc : processors)
    {
        proc->finishPlayback();
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

    for (int i = 0; i < size; ++i)
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
    if(JT_FIX(newValue) < getNumProcessors())
    {
        selectedIndex = newValue;
    }
    else
    {
        selectedIndex = 0;
    }
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
