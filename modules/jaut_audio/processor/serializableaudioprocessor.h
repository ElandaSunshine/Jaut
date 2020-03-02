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
    @file   serializableaudioprocessor.h
    @date   08, February 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
class SerializableAudioProcessor : public AudioProcessor 
{
public:
    SerializableAudioProcessor() = default;

    //==================================================================================================================
    virtual void readData(const ValueTree state) = 0;
    virtual void writeData(ValueTree state) = 0;

    //==================================================================================================================
    void getStateInformation(juce::MemoryBlock &destData) override { ignoreUnused(destData); }
    void setStateInformation(const void *data, int sizeInBytes) override { ignoreUnused(data, sizeInBytes); }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 0; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override { ignoreUnused(index); }
    const String getProgramName(int index) override { ignoreUnused(index); return ""; }
    void changeProgramName(int index, const String &newName) override { ignoreUnused(index, newName); }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    void reset() override {}
    AudioProcessorEditor *createEditor() override {}
    bool isBusesLayoutSupported(const AudioProcessor::BusesLayout &layout) const override
    { ignoreUnused(layout); return true; }

    JUCE_DECLARE_NON_COPYABLE(SerializableAudioProcessor)
};

template<class Processor = AudioProcessor>
class SerializableProcessorWrapper : public SerializableAudioProcessor
{
public:
    explicit SerializableProcessorWrapper(Processor &processor)
            : processor(*processor, false)
    {
        jassert((std::is_base_of_v<AudioProcessor, Processor>));
    }

    explicit SerializableProcessorWrapper(Processor *processor, bool isOwned = true)
        : processor(processor, isOwned)
    {
        jassert(processor != nullptr);
        jassert((std::is_base_of_v<AudioProcessor, Processor>));
    }

    //==================================================================================================================
    Processor &getProcessor() const
    {
        return *processor;
    }

    //==================================================================================================================
    virtual void readData(const ValueTree state) override
    {
        MemoryOutputStream stream;
        state.writeToStream(stream);
        processor->setStateInformation(stream.getData(), stream.getDataSize());
    }

    virtual void writeData(ValueTree state) override
    {
        MemoryBlock data;
        processor->getStateInformation(data);
        state.readFromData(data.getData(), data.getSize());
    }

private:
    OptionalScopedPointer<Processor> processor;
    JUCE_DECLARE_NON_COPYABLE(SerializableProcessorWrapper)
};
}
