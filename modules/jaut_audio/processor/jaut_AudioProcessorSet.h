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
    @file   jaut_AudioProcessorSet.h
    @date   10, July 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_AssertDef.h>
#include <jaut_core/util/jaut_TypeContainer.h>
#include "jaut_core/math/jaut_MathUtil.h"

#include <jaut_audio/processor/jaut_ISerialisableAudioProcessor.h>



namespace jaut
{
    //==================================================================================================================
    /**
     *  The set processor allows to define a static set of processors that will be processed at processing time.
     *  Other than juce's juce::dsp::ProcessorChain, however, this does not process all processors but only a selected
     *  instance.
     *  
     *  @tparam Processors The static list of processors,
     *                     every processor needs to be a valid jaut::ISerialisableAudioProcessor
     */
    template<class ...Processors>
    class AudioProcessorSet : public ISerialisableAudioProcessor
    {
    public:
        static_assert((std::is_base_of_v<ISerialisableAudioProcessor, std::decay_t<Processors>> && ...),
                      JAUT_ASSERT_AUDIO_PROCESSOR_SET_NOT_I_SERIALISABLE_AUDIO_PROCESSOR);
        static_assert(sizeof...(Processors) > 0, JAUT_ASSERT_SERIALISABLE_JUCE_PROCESSOR_PROXY_IS_ABSTRACT);
        
        //==============================================================================================================
        /** The type that represents the list of processors as a TypeArray. */
        using ProcessorList = TypeArray<Processors...>;
        
        /** The array containing the processors. */
        using ProcessorArray = typename ProcessorList::toArray;
    
        //==============================================================================================================
        static constexpr int numProcessors = sizeof...(Processors);
        
        //==============================================================================================================
        template<class Fn>
        explicit AudioProcessorSet(Fn                                      &&initialiserCallback,
                                   const juce::ParameterID                 &indexParameterId,
                                   const juce::String                      &indexParameterName,
                                   const juce::AudioParameterIntAttributes &indexParameterAttributes = {});
        
        //==============================================================================================================
        void prepare(ProcessSpec spec) override;
        void release() override;
        void process(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override;
        void process(juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiMessages) override;
        void reset() override;
        
        //==============================================================================================================
        juce::String getName() const override;
        
        //==============================================================================================================
        /**
         *  Gets the created audio parameter for this processor, which will hold the selected index.
         *  @return The parameter
         */
        JAUT_NODISCARD
        juce::AudioParameterInt& getSelectedIndexParameter() noexcept;

        /**
         *  Gets the created audio parameter for this processor, which will hold the selected index.
         *  @return The parameter
         */
        JAUT_NODISCARD
        const juce::AudioParameterInt& getSelectedIndexParameter() const noexcept;
    
        //==============================================================================================================
        JAUT_NODISCARD
        int size() const noexcept;
        
        //==============================================================================================================
        void readData(juce::ValueTree state) override;
        void writeData(juce::ValueTree state) override;
        
    private:
        template<int I = 0>
        static constexpr auto getProcessorFromIndex(const typename ProcessorArray::value_type &var, int index) noexcept
            -> ISerialisableAudioProcessor&
        {
            if (index == I)
            {
                return std::get<I>(var);
            }
            
            return getProcessorFromIndex<I + 1>(var, index);
        }
        
        //==============================================================================================================
        ProcessorArray processors;
        
        ISerialisableAudioProcessor *current;
        juce::AudioParameterInt     paramSelectedProcessor;
        
        int lastProcessor { 0 };
        
        //==============================================================================================================
        void swapProcessors(int currentIndex)
        {
            auto &var = processors[static_cast<std::size_t>(currentIndex)];
            
            current       = &getProcessorFromIndex(var, currentIndex);
            lastProcessor = currentIndex;
        }
        
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE(AudioProcessorSet)
    };
    
    //==================================================================================================================
    // IMPLEMENTATION
    template<class ...T>
    template<class Fn>
    inline AudioProcessorSet<T...>::AudioProcessorSet(Fn                      &&parInitialiserCallback,
                                                      const juce::ParameterID &parIndexParameterId,
                                                      const juce::String      &parIndexParameterName,
                                                      const juce::AudioParameterIntAttributes
                                                          &parIndexParameterAttributes)
        : processors { std::forward<Fn>(parInitialiserCallback)() },
          current(&std::get<0>(processors.at(0))),
          paramSelectedProcessor(parIndexParameterId, parIndexParameterName, 0, (sizeof...(T) - 1), 0,
                                 parIndexParameterAttributes)
    {}
    
    //==================================================================================================================
    template<class ...T>
    inline void AudioProcessorSet<T...>::prepare(ProcessSpec parSpec)
    {
        ((std::get<ProcessorList::template indexOf<T>>
            (processors.at(ProcessorList::template indexOf<T>)).prepare(parSpec)), ...);
    }
    
    template<class ...T>
    inline void AudioProcessorSet<T...>::release()
    {
        ((std::get<ProcessorList::template indexOf<T>>
            (processors.at(ProcessorList::template indexOf<T>)).release()), ...);
    }
    
    template<class ...T>
    inline void AudioProcessorSet<T...>::process(juce::AudioBuffer<float> &parBuffer, juce::MidiBuffer &parMidiMessages)
    {
        if (const int par_index = paramSelectedProcessor.get(); par_index != lastProcessor)
        {
            swapProcessors(par_index);
        }
        
        current->process(parBuffer, parMidiMessages);
    }
    
    template<class ...T>
    inline void AudioProcessorSet<T...>::process(juce::AudioBuffer<double> &parBuffer,
                                                 juce::MidiBuffer          &parMidiMessages)
    {
        if (const int par_index = paramSelectedProcessor.get(); par_index != lastProcessor)
        {
            swapProcessors(par_index);
        }
        
        current->process(parBuffer, parMidiMessages);
    }
    
    template<class ...T>
    inline void AudioProcessorSet<T...>::reset()
    {
        ((std::get<ProcessorList::template indexOf<T>>
            (processors.at(ProcessorList::template indexOf<T>)).reset()), ...);
    }
    
    //==================================================================================================================
    template<class ...T>
    inline juce::String AudioProcessorSet<T...>::getName() const
    {
        return "jaut:AudioProcessorSet";
    }

    //==================================================================================================================
    template<class ...T>
    inline juce::AudioParameterInt& AudioProcessorSet<T...>::getSelectedIndexParameter() noexcept
    {
        return paramSelectedProcessor;
    }

    template<class ...T>
    inline const juce::AudioParameterInt& AudioProcessorSet<T...>::getSelectedIndexParameter() const noexcept
    {
        return paramSelectedProcessor;
    }

    //==================================================================================================================
    template<class ...T>
    inline int AudioProcessorSet<T...>::size() const noexcept
    {
        return numProcessors;
    }

    //==================================================================================================================
    template<class ...T>
    inline void AudioProcessorSet<T...>::readData(juce::ValueTree parState)
    {
        for (const auto &processor_state : parState)
        {
            if (!processor_state.hasType("Processor"))
            {
                continue;
            }
            
            if (const int index = processor_state.getProperty("id", -1); fit<int>(index, 0, processors.size()))
            {
                ISerialisableAudioProcessor &processor = getProcessorFromIndex(processors[index], index);
                processor.readData(processor_state);
            }
        }
        
        const int selected_index = parState.getProperty("selected", 0);
        paramSelectedProcessor   = std::clamp<int>(selected_index, 0, (size() - 1));
    }
    
    template<class ...T>
    inline void AudioProcessorSet<T...>::writeData(juce::ValueTree parState)
    {
        parState.removeAllChildren(nullptr);
        
        for (int i = 0; i < static_cast<int>(processors.size()); ++i)
        {
            juce::ValueTree processor_state("Processor");
            
            ISerialisableAudioProcessor &processor = getProcessorFromIndex(processors[i], i);
            processor.writeData(processor_state);
            processor_state.setProperty("id", i, nullptr);

            parState.appendChild(processor_state, nullptr);
        }
        
        const int selected_index = paramSelectedProcessor.get();
        parState.setProperty("selected", selected_index, nullptr);
    }
}
