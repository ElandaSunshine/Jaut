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
/**
 *  Classes inheriting from this class become new AudioProcessor with two additional functions: readData(), writeData()
 *  These are for saving their state instead of get/setStateInformation.
 *
 *  This class is mostly used for child processors, as root processors still need to implement get/setStateInformation
 *  to save their state at all.
 *  This is just a shortcut that can be used to transfer the state through several nested child-trees.
 */
class SerializableAudioProcessor : public juce::AudioProcessor
{
public:
    SerializableAudioProcessor() = default;

    //==================================================================================================================
    /**
     *  Reads data from the loaded state and applies it to the audio processor.
     *  @param state The state object to read from
     */
    virtual void readData(const juce::ValueTree state) = 0;
    
    /**
     *  Write data to the state object which was read from the audio processor.
     *  @param state The state object to write to
     */
    virtual void writeData(juce::ValueTree state) = 0;

    //==================================================================================================================
    void getStateInformation(juce::MemoryBlock &destData) override { ignoreUnused(destData); }
    void setStateInformation(const void *data, int sizeInBytes) override { juce::ignoreUnused(data, sizeInBytes); }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 0; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override { juce::ignoreUnused(index); }
    const juce::String getProgramName(int index) override { juce::ignoreUnused(index); return ""; }
    void changeProgramName(int index, const juce::String &newName) override { juce::ignoreUnused(index, newName); }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    void reset() override {}
    juce::AudioProcessorEditor *createEditor() override { return nullptr; }
    bool isBusesLayoutSupported(const juce::AudioProcessor::BusesLayout &layout) const override
    { juce::ignoreUnused(layout); return true; }

    JUCE_DECLARE_NON_COPYABLE(SerializableAudioProcessor)
};

/**
 *  The SerializableProcessorWrapper is a SerializableAudioProcessor wrapper that wraps any other instance of
 *  an AudioProcessor into a sort of shell-like structure, that will act as if the encapsulated AudioProcessor
 *  was a SerializableAudioProcessor from the beginning.
 *
 *  The advantages of this is that get/setStateInformation will be substituted with read/writeData and will
 *  manage state data for you internally.
 *
 *  @tparam Processor The AudioProcessor type to encapsulate
 */
template<class Processor = juce::AudioProcessor>
class SerializableProcessorWrapper : public SerializableAudioProcessor
{
public:
    /**
     *  Creates a shallow wrapper which will keep a reference to the processor object to manage state data.
     *  @param processor The AudioProcessor to reference
     */
    explicit SerializableProcessorWrapper(Processor &processor)
        : processor(*processor, false)
    {
        jassert((std::is_base_of_v<juce::AudioProcessor, Processor>));
    }
    
    /**
     *  Creates either a shallow wrapper which will keep a reference to the processor object,
     *  or a capsule wrapper that will own the processor object and delete it when this wrapper is destroyed.
     *
     *  @param processor The AudioProcessor to reference
     *  @param isOwned True if the processor should be owned and deleted
     */
    explicit SerializableProcessorWrapper(Processor *processor, bool isOwned = true)
        : processor(processor, isOwned)
    {
        jassert(processor != nullptr);
        static_assert(std::is_base_of_v<juce::AudioProcessor, Processor>, "Type-parameter Processor must be of "
                                                                          "type AudioProcessor.");
    }

    //==================================================================================================================
    /**
     *  Gets the current holding AudioProcessor object.
     *  If the wrapper is holding a nullptr, this will most likely crash your program.
     *  If you aren't certain that this will be valid, it is best to check with isValid() first and then
     *  get the processor object.
     *
     *  @return The processor object
     */
    Processor &getProcessor() const noexcept
    {
        return *processor;
    }

    /**
     *  Determines whether the current processor object is valid.
     *  @return True if the processor points somewhere, false if not
     */
    bool isValid() const noexcept
    {
        return processor != nullptr;
    }
    
    //==================================================================================================================
    virtual void readData(const juce::ValueTree state) override
    {
        juce::MemoryOutputStream stream;
        state.writeToStream(stream);
        processor->setStateInformation(stream.getData(), stream.getDataSize());
    }

    virtual void writeData(juce::ValueTree state) override
    {
        juce::MemoryBlock data;
        processor->getStateInformation(data);
        state.copyPropertiesFrom(juce::ValueTree::readFromData(data.getData(), data.getSize()), nullptr);
    }

private:
    juce::OptionalScopedPointer<Processor> processor;
    JUCE_DECLARE_NON_COPYABLE(SerializableProcessorWrapper)
};
}
