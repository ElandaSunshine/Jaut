/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any internal version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   SfxUnit.h
    @date   23, December 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
class JAUT_API SfxUnit : public SerializableAudioProcessor
{
public:
    struct JAUT_API DataContext {};

    struct JAUT_API SfxParameter final
    {
        const String parameterID;
        const String parameterName;
        const String labelText;
        const NormalisableRange<float> valueRange;
        const float defaultValue;
        const std::function<String(float)> valueToText;
        const std::function<float(const String&)> textToValue;
        const AudioProcessorParameter::Category parameterCategory = AudioProcessorParameter::genericParameter;
        const bool isMetaParameter;
        const bool isAutomatableParameter;
        const bool isDiscrete;
        const bool isBoolean;

        //==============================================================================================================
        explicit SfxParameter(const String &id, const String& name, const String& label,
                              NormalisableRange<float> range, float defaultValue,
                              std::function<String(float)> toTextFunction,
                              std::function<float(const String&)> fromTextFunction, bool isMetaParameter = false,
                              bool isAutomatableParameter = true, bool isDiscrete = false,
                              AudioProcessorParameter::Category category = AudioProcessorParameter::genericParameter,
                              bool isBoolean = false)
            : parameterID(id), parameterName(name), labelText(label), valueRange(range), defaultValue(defaultValue),
              valueToText(toTextFunction), textToValue(fromTextFunction), parameterCategory(category),
              isMetaParameter(isMetaParameter), isAutomatableParameter(isAutomatableParameter), isDiscrete(isDiscrete),
              isBoolean(isBoolean)
        {}
    };

    enum JAUT_API UnitType
    {
        Effector,
        Modulator
    };

    //==================================================================================================================
    SfxUnit(DspUnit &unit, AudioProcessorValueTreeState &vts, UndoManager *undoManager= nullptr);

    //==================================================================================================================
    virtual void processEffect(int index, AudioBuffer<float> &buffer,  MidiBuffer &midiBuffer) = 0;
    virtual void processEffect(int index, AudioBuffer<double> &buffer, MidiBuffer &midiBuffer) = 0;
    virtual void beginPlayback(int index, double sampleRate, int bufferSize) = 0;
    virtual void finishPlayback(int index) = 0;
    virtual std::vector<SfxParameter> createParameters() const = 0;
    virtual int getMaxInstances() const = 0;

    //==================================================================================================================
    virtual UnitType getProcessingType() const { return Effector; }
    virtual DataContext *getNewContext() const { return nullptr; }

protected:
    RangedAudioParameter *getParameter(int index, const String &parameterId);
    DataContext *getDataContext(int index);
    void initialize();

private:
    OwnedArray<DataContext> contexts;

    //==================================================================================================================
    void process(AudioBuffer<float> &buffer,  MidiBuffer &midiBuffer) override;
    void process(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer) override;
    void beginPlayback(double sampleRate, int bufferSize) override;
    void finishPlayback() override;

    //==================================================================================================================
    void readData(const ValueTree data) override { ignoreUnused(data); }
    void writeData(ValueTree data) const override { ignoreUnused(data); }

    JUCE_DECLARE_NON_COPYABLE(SfxUnit)
};
}
