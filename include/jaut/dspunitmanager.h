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
 * File: dspunitmanager.h
 * Time: 25, June 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/dspunit.h>

namespace jaut
{
class JAUT_API DspUnitManager final : public DspUnit, private AsyncUpdater,
                                      PropertyMap::Listener
{
public:
    //=================================================================================================================
    DspUnitManager(AudioProcessor &parentProcessor, AudioProcessorValueTreeState &vts,
                   UndoManager *undoManager = nullptr, int initialCapacity = 0,
                   bool unitsAreOwned = true) noexcept;

    ~DspUnitManager() override;

    //=================================================================================================================
    void addProcessorType(DspUnit *processor, bool notifyGui = true) noexcept;
    DspUnit *getCurrentProcessor();
    void setCurrentProcessor(int processorIndex);

    //=================================================================================================================
    const int getNumProcessors() const noexcept;
    const String getName() const override;
    bool hasEditor() const override;
    const bool doesOwnUnits() const noexcept;

    //=================================================================================================================
    void process(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer) override;
    void process(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer) override;
    void beginPlayback(double sampleRate, int maxBlockSamples) override;
    void finishPlayback() override;
    void reset() override;

    //=================================================================================================================
    void readData(const ValueTree data) override;
    void writeData(ValueTree data) const override;

private:
    friend class GuiDspUnitManager;

    Array<DspUnit*> processors;
    int selectedIndex;
    int storage;
    bool isOwned;

    //=================================================================================================================
    void onValueChanged(const String &name, var oldValue, var newValue) override;
    void onPropertyAdded(const String &name, var value) override {}
    DspGui *getGuiType() override;
    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DspUnitManager)
};
}
