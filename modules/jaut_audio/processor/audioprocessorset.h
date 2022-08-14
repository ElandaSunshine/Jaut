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
    @file   audioprocessorset.h
    @date   10, July 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
template<class ...Processors>
class AudioProcessorSet : public SerialisableAudioProcessor
{
public:
    static_assert((std::is_base_of_v<ISerialisableAudioProcessor,
                                     std::remove_pointer_t<std::decay_t<Processors>>> && ...),
                  "Processors must inherit from jaut::SerializableAudioProcessor");
    static_assert(sizeof...(Processors) > 0, "Must have at least one processor");
    
    //==================================================================================================================
    using ProcessorList  = TypeArray<Processors...>;
    using ProcessorArray = typename ProcessorList::toArray;
    using VariantType    = typename ProcessorList::template to<std::variant>;
    
    //==================================================================================================================
    template<class Type, class ...Args>
    static VariantType makeProcessor(Args &&...args)
    {
        return VariantType(std::in_place_type_t<Type>{}, std::forward<Args>(args)...);
    }
    
    //==================================================================================================================
    template<class Fn, class = std::enable_if<not std::is_same_v<Fn, AudioProcessorSet>>>
    explicit AudioProcessorSet(Fn &&initialiserCallback)
        : processors { std::forward<Fn>(initialiserCallback)() }
    {}
    
    //==================================================================================================================
    void prepare(ProcessSpec spec) override
    {
        ((std::get<Processors>(processors.at(ProcessorList::template indexOf<Processors>)).prepare(spec)), ...);
    
        if (const int par_index = parSelectedProcessor->get(); par_index != lastProcessor)
        {
            swapProcessors(par_index);
        }
    }
    
    void release() override
    {
        ((std::get<Processors>(processors.at(ProcessorList::template indexOf<Processors>)).release()), ...);
    }
    
    void process(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override
    {
        if (const int par_index = parSelectedProcessor->get(); par_index != lastProcessor)
        {
            swapProcessors(par_index);
        }
    
        current->process(buffer, midiMessages);
    }
    
    void reset() override
    {
        ((std::get<Processors>(processors.at(ProcessorList::template indexOf<Processors>)).reset()), ...);
    }
    
    //==================================================================================================================
    juce::String getName() const override
    {
        return "AudioProcessorSet";
    }
    
    //==================================================================================================================
    juce::AudioParameterInt *createParameter(const juce::String &id, const juce::String &name,
                                             const juce::String &label = juce::String(),
                                             std::function<juce::String(int, int)>   stringFromInt = nullptr,
                                             std::function<int(const juce::String&)> intFromString = nullptr)
    {
        if (!parSelectedProcessor)
        {
            parSelectedProcessor = std::make_unique<juce::AudioParameterInt>(id, name, 0, ProcessorList::size - 1, 0,
                                                                             label, stringFromInt, intFromString);
        }
        
        return parSelectedProcessor.get();
    }
    
    //==================================================================================================================
    void readData(juce::ValueTree state) override
    {
        for (const auto &processor_state : state)
        {
            if (!processor_state.hasType("Processor"))
            {
                continue;
            }
            
            if (const int index = state.getProperty("id", -1); fit<int>(index, 0, processors.size()))
            {
                std::visit
                (
                    [&processor_state](ISerialisableAudioProcessor &processor)
                    {
                        processor.readData(processor_state);
                    },
                    processors.at(static_cast<SizeTypes::Array>(index))
                );
            }
        }
    }
    
    void writeData(juce::ValueTree state) override
    {
        state.removeAllChildren(nullptr);
        
        for (int i = 0; i < static_cast<int>(processors.size()); ++i)
        {
            juce::ValueTree processor_state("Processor");
            
            std::visit
            (
                [&processor_state](ISerialisableAudioProcessor &processor)
                {
                    processor.writeData(processor_state);
                },
                processors.at(static_cast<SizeTypes::Array>(i))
            );
            
            processor_state.setProperty("id", i, nullptr);
            state.appendChild(processor_state, nullptr);
        }
    }
    
private:
    ProcessorArray processors;
    ISerialisableAudioProcessor *current { &std::get<typename ProcessorList::template at<0>>(processors.at(0)) };
    std::unique_ptr<juce::AudioParameterInt> parSelectedProcessor;
    int lastProcessor { 0 };
    
    //==================================================================================================================
    void swapProcessors(int currentIndex)
    {
        std::visit
        (
            [this](ISerialisableAudioProcessor &processor)
            {
                current = &processor;
            },
            processors.at(static_cast<SizeTypes::Array>(currentIndex))
        );
    
        lastProcessor = currentIndex;
    }
    
    JUCE_DECLARE_NON_COPYABLE(AudioProcessorSet)
};
}
