/**
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
                     ░░░░░██╗░█████╗░██╗░░░██╗████████╗
                     ░░░░░██║██╔══██╗██║░░░██║╚══██╔══╝
                     ░░░░░██║███████║██║░░░██║░░░██║░░░
                     ██╗░░██║██╔══██║██║░░░██║░░░██║░░░
                     ╚█████╔╝██║░░██║╚██████╔╝░░░██║░░░
                     ░╚════╝░╚═╝░░╚═╝░╚═════╝░░░░╚═╝░░░
                       JUCE Augmented Utility  Toolbox
    ─────────────────────────────── ⋆⋅☆⋅⋆ ───────────────────────────────
    
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
    @file   jaut_SerialisableJuceProcessorProxy.h
    @date   08, February 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_AssertDef.h>
#include <jaut_audio/processor/jaut_ISerialisableAudioProcessor.h>



namespace jaut
{
    //==================================================================================================================
    /**
     *  The SerialisableJuceProcessorProxy is an ISerializableAudioProcessor wrapper that wraps any other instance of
     *  an AudioProcessor into a sort of shell-like structure, that will act as if the encapsulated AudioProcessor
     *  was a SerializableAudioProcessor from the beginning.
     *
     *  The advantages of this is that get/setStateInformation will be substituted with read/writeData and will
     *  manage state data for you internally.
     *
     *  @tparam InternalAudioProcessor The AudioProcessor type to encapsulate
     */
    template<class InternalAudioProcessor>
    class SerialisableJuceProcessorProxy : public ISerialisableAudioProcessor
    {
    public:
        static_assert(std::is_base_of_v<juce::AudioProcessor, InternalAudioProcessor>,
                      JAUT_ASSERT_SERIALISABLE_JUCE_PROCESSOR_PROXY_NOT_AUDIO_PROCESSOR);
        static_assert(!std::is_abstract_v<InternalAudioProcessor>,
                      JAUT_ASSERT_SERIALISABLE_JUCE_PROCESSOR_PROXY_IS_ABSTRACT);
        
        //==============================================================================================================
        /**
         *  Constructs a new SerialisableJuceProcessorProxy.
         *  The arguments are the ones you would pass to the InternalAudioProcessor.
         *  
         *  @param args InternalAudioProcessor construction arguments
         */
        template<class ...Args>
        explicit SerialisableJuceProcessorProxy(Args &&...args);
        
        //==============================================================================================================
        void process(juce::AudioBuffer<float>  &buffer, juce::MidiBuffer &midiBuffer) override;
        void process(juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiBuffer) override;
        
        //==============================================================================================================
        JAUT_NODISCARD
        juce::String getName() const override;
        
        //==============================================================================================================
        JAUT_NODISCARD
        std::unique_ptr<juce::Component> createComponent() const override;
        
        //==============================================================================================================
        void prepare(ProcessSpec specs) override;
        void release() override;
        void reset() override;
        
        //==============================================================================================================
        void readData(juce::ValueTree state) override;
        void writeData(juce::ValueTree state) override;
    
        //==============================================================================================================
        /**
         *  Gets the underlying processor.
         *  @return The AudioProcessor
         *  
         *  @see juce::AudioProcessor
         */
        JAUT_NODISCARD
        InternalAudioProcessor* getJuceProcessor() noexcept;
    
        /**
         *  Gets the underlying processor.
         *  @return The AudioProcessor
         *  
         *  @see juce::AudioProcessor
         */
        JAUT_NODISCARD
        const InternalAudioProcessor* getJuceProcessor() const noexcept;
        
    public:
        InternalAudioProcessor            internalAudioProcessor;
        juce::AudioProcessor::BusesLayout busesLayout;
    };
    
    //==================================================================================================================
    // IMPLEMENTATION
    template<class T>
    template<class ...Args>
    inline SerialisableJuceProcessorProxy<T>::SerialisableJuceProcessorProxy(Args &&...parArgs)
        : internalAudioProcessor(std::forward<Args>(parArgs)...),
          busesLayout(internalAudioProcessor.getBusesLayout())
    {}
    
    //==================================================================================================================
    template<class T>
    inline void SerialisableJuceProcessorProxy<T>::process(juce::AudioBuffer<float> &parBuffer,
                                                           juce::MidiBuffer         &parMidiBuffer)
    {
        internalAudioProcessor.processBlock(parBuffer, parMidiBuffer);
    }
    
    template<class T>
    inline void SerialisableJuceProcessorProxy<T>::process(juce::AudioBuffer<double> &parBuffer,
                                                           juce::MidiBuffer          &parMidiBuffer)
    {
        internalAudioProcessor.processBlock(parBuffer, parMidiBuffer);
    }
    
    //==================================================================================================================
    template<class T>
    inline juce::String SerialisableJuceProcessorProxy<T>::getName() const
    {
        return internalAudioProcessor.getName();
    }
    
    //==================================================================================================================
    template<class T>
    inline std::unique_ptr<juce::Component> SerialisableJuceProcessorProxy<T>::createComponent() const
    {
        return (internalAudioProcessor->hasEditor()
                    ? internalAudioProcessor->createEditor()
                    : ISerialisableAudioProcessor::createComponent());
    }
    
    //==================================================================================================================
    template<class T>
    inline void SerialisableJuceProcessorProxy<T>::prepare(ProcessSpec parSpecs)
    {
        internalAudioProcessor.setRateAndBufferSizeDetails(parSpecs.sampleRate, parSpecs.maxSamplesPerBlock);
        internalAudioProcessor.prepareToPlay(parSpecs.sampleRate, parSpecs.maxSamplesPerBlock);
    }
    
    template<class T>
    inline void SerialisableJuceProcessorProxy<T>::release()
    {
        internalAudioProcessor.releaseResources();
    }

    template<class T>
    inline void SerialisableJuceProcessorProxy<T>::reset()
    {
        internalAudioProcessor.reset();
    }
    
    //==================================================================================================================
    template<class T>
    inline void SerialisableJuceProcessorProxy<T>::readData(juce::ValueTree parState)
    {
        juce::MemoryOutputStream output_stream;
        parState.writeToStream(output_stream);
        internalAudioProcessor.setStateInformation(output_stream.getData(), output_stream.getDataSize());
    }

    template<class T>
    inline void SerialisableJuceProcessorProxy<T>::writeData(juce::ValueTree parState)
    {
        juce::MemoryBlock memory_block;
        internalAudioProcessor.getStateInformation(memory_block);
        parState.copyPropertiesAndChildrenFrom(juce::ValueTree::readFromData(memory_block.getData(),
                                                                             memory_block.getSize()),
                                                                            nullptr);
    }
    
    //==================================================================================================================
    template<class InternalAudioProcessor>
    inline InternalAudioProcessor* SerialisableJuceProcessorProxy<InternalAudioProcessor>::getJuceProcessor() noexcept
    {
        return internalAudioProcessor;
    }
    
    template<class InternalAudioProcessor>
    inline auto SerialisableJuceProcessorProxy<InternalAudioProcessor>::getJuceProcessor() const noexcept
        -> const InternalAudioProcessor*
    {
        return internalAudioProcessor;
    }
}
