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
 * File: dspunit.h
 * Time: 11, July 2019
 *
 * ===============================================================
 */

#pragma once

#include <jaut/expo.h>
#include <jaut/propertymap.h>

namespace jaut
{
class DspGui;
class JAUT_API DspUnit : public AudioProcessor
{
public:
    //=================================================================================================================
    using t_parameter = AudioProcessorValueTreeState::Parameter;

    //=================================================================================================================
    DspUnit(DspUnit &processor, AudioProcessorValueTreeState *vts = nullptr,
            UndoManager *undoManager = nullptr) noexcept;
    DspUnit(AudioProcessor &processor, AudioProcessorValueTreeState *vts = nullptr,
            UndoManager *undoManager = nullptr) noexcept;
    virtual ~DspUnit();

    //=================================================================================================================
    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer) final override;
    void processBlock(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer) final override;
    virtual void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) final override;
    virtual void releaseResources() final override;
    DspGui *createGui();
    ValueTree readFromData(const void *data, int sizeInBytes) const;
    void writeToMemory(ValueTree data, MemoryBlock &block) const;

    //=================================================================================================================
    virtual const String getName() const override = 0;
    virtual bool hasEditor() const override = 0;
    virtual void process(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer) = 0;
    virtual void process(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer) {}
    virtual void beginPlayback(double sampleRate, int maxBlockSamples) = 0;
    virtual void finishPlayback() = 0;
    virtual void readData(const ValueTree data) = 0;
    virtual void writeData(ValueTree data) const = 0;

    //=================================================================================================================
    virtual double getTailLengthSeconds() const override { return 0.0; }
    virtual int getNumPrograms() override { return 0; }
    virtual int getCurrentProgram() override { return 0; }
    virtual void setCurrentProgram(int index) override { ignoreUnused(index); }
    virtual const String getProgramName(int index) override { ignoreUnused(index); return ""; }
    virtual void changeProgramName(int index, const String &newName) override { ignoreUnused(index, newName); }
    virtual bool acceptsMidi() const override { return false; }
    virtual bool producesMidi() const override { return false; }
    virtual void reset() override {}
    void getStateInformation(MemoryBlock &destData) override { ignoreUnused(destData); }
    void setStateInformation(const void *data, int sizeInBytes) override { ignoreUnused(data, sizeInBytes); }
    virtual bool isBusesLayoutSupported(const AudioProcessor::BusesLayout &layout) const override
    { ignoreUnused(layout); return true; }

protected:
    AudioProcessor &processor;
    UndoManager *history;
    DspGui *activeGui;
    AudioProcessorValueTreeState *parameters;

    PropertyMap properties;

    //=================================================================================================================
    virtual DspGui *getGuiType() = 0;
    RangedAudioParameter *createParameter(std::unique_ptr<t_parameter> par);
    RangedAudioParameter *createParameter(const String &parId, const String &parName, const String &parLabel,
                                          const NormalisableRange<float> &range, float defaultValue,
                                          std::function<String(float)> valueToTextFunction,
                                          std::function<float(const String &)> textToValueFunction, bool isMetaParameter = false,
                                          bool isAutomatableParameter = true, bool isDiscrete = false,
                                          AudioProcessorParameter::Category category = AudioProcessorParameter::genericParameter,
                                          bool isBoolean = false);
    RangedAudioParameter *getParameter(const String &parId) noexcept;

private:
    friend class DspGui;

    //=================================================================================================================
    void initialize() noexcept;
    AudioProcessorEditor *createEditor() override final { return 0; }

    JUCE_DECLARE_NON_COPYABLE(DspUnit)
};
}
