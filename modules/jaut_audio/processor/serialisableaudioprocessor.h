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
    
    Copyright (c) 2022 ElandaSunshine
    ===============================================================
    
    @author Elanda
    @file   serializableaudioprocessor.h
    @date   08, February 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
struct ISerialisableAudioProcessor
{
    virtual ~ISerialisableAudioProcessor() = default;
    
    //==================================================================================================================
    /**
     *  Processes the current block with single-precisions and gives back the modified data.
     *
     *  @param buffer     The buffer containing the sample data
     *  @param midiBuffer The buffer containing the midi messages
     */
    virtual void process(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiBuffer) = 0;
    
    /**
     *  Processes the current block with double-precisions and gives back the modified data.
     *
     *  @param buffer     The buffer containing the sample data
     *  @param midiBuffer The buffer containing the midi messages
     */
    virtual void process(juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiBuffer) {}
    
    //==================================================================================================================
    /**
     *  Gets the name of this processor object.
     *  @return The processor name
     */
    virtual juce::String getName() const = 0;
    
    //==================================================================================================================
    /**
     *  If your processor has a special component associated with it, you can create it directly from the processor.     *
     *  @return The component
     */
    virtual std::unique_ptr<juce::Component> createComponent() = 0;
    
    //==================================================================================================================
    /**
     *  This is called before any playback is about to happen.
     *  Mainly used for preparing resources like allocating buffers and/or storing temporary values that are
     *  needed inside the processing routine.
     *
     *  @param specs A struct containing basic records about playback
     */
    virtual void prepare(ProcessSpec specs) = 0;
    
    /**
     *  This is called after any playback has happened.
     *  Here the resources previously used can be cleaned up and deallocated, made ready for the next playback.
     */
    virtual void release() = 0;
    
    /**
     *  It is unclear when this is called by the host or if it is called at all.
     *  See juce::AudioProcessor::reset() for a better explanation on how this method behaves.
     */
    virtual void reset() {}
    
    //==================================================================================================================
    /**
     *  Reads data from the loaded state and applies it to the audio processor.
     *  @param state The state object to read from
     */
    virtual void readData(juce::ValueTree state) = 0;
    
    /**
     *  Write data to the state object which was read from the audio processor.
     *  @param state The state object to write to
     */
    virtual void writeData(juce::ValueTree state) = 0;
};

class SerialisableAudioProcessor : public ISerialisableAudioProcessor
{
public: std::unique_ptr<juce::Component> createComponent() override { return nullptr; }
};

/**
 *  The SerialisableJuceProcessorProxy is a SerializableAudioProcessor wrapper that wraps any other instance of
 *  an AudioProcessor into a sort of shell-like structure, that will act as if the encapsulated AudioProcessor
 *  was a SerializableAudioProcessor from the beginning.
 *
 *  The advantages of this is that get/setStateInformation will be substituted with read/writeData and will
 *  manage state data for you internally.
 *
 *  @tparam Processor The AudioProcessor type to encapsulate
 */
template<class InternalAudioProcessor>
class SerialisableJuceProcessorProxy : public ISerialisableAudioProcessor
{
public:
    static_assert(std::is_base_of_v<juce::AudioProcessor, InternalAudioProcessor>,
                  "Type AudioProcessorClass must be of type juce::AudioProcessor");
    static_assert(!std::is_abstract_v<InternalAudioProcessor>,
                  "Type AudioProcessorClass cannot be an abstract class");
    
    //==================================================================================================================
    template<class ...Args>
    explicit SerialisableJuceProcessorProxy(Args &&...args)
        : internalAudioProcessor(std::forward<Args>(args)...),
          busesLayout(internalAudioProcessor.getBusesLayout())
    {}
    
    //==================================================================================================================
    void process(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiBuffer) override
    {
        
        internalAudioProcessor.processBlock(buffer, midiBuffer);
    }
    
    void process(juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiBuffer) override
    {
        internalAudioProcessor.processBlock(buffer, midiBuffer);
    }
    
    //==================================================================================================================
    juce::String getName() const override
    {
        return juce::String("Proxied;") + internalAudioProcessor.getName();
    }
    
    //==================================================================================================================
    std::unique_ptr<juce::Component> createComponent() override
    {
        return internalAudioProcessor.hasEditor() ? internalAudioProcessor.createEditor() : nullptr;
    }
    
    //==================================================================================================================
    void prepare(ProcessSpec specs) override
    {
        internalAudioProcessor.setRateAndBufferSizeDetails(specs.sampleRate, specs.maxSamplesPerBlock);
        internalAudioProcessor.prepareToPlay(specs.sampleRate, specs.maxSamplesPerBlock);
    }
    
    void release() override
    {
        internalAudioProcessor.releaseResources();
    }
    
    void reset() override
    {
        internalAudioProcessor.reset();
    }
    
    //==================================================================================================================
    void readData(juce::ValueTree state) override
    {
        juce::MemoryOutputStream output_stream;
        state.writeToStream(output_stream);
        internalAudioProcessor.setStateInformation(output_stream.getData(), output_stream.getDataSize());
    }
    
    void writeData(juce::ValueTree state) override
    {
        juce::MemoryBlock memory_block;
        internalAudioProcessor.getStateInformation(memory_block);
        state.copyPropertiesAndChildrenFrom(juce::ValueTree::readFromData(memory_block.getData(),
                                                                          memory_block.getSize()), nullptr);
    }
    
public:
    InternalAudioProcessor internalAudioProcessor;
    juce::AudioProcessor::BusesLayout busesLayout;
};
}
