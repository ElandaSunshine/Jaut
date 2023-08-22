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
    @file   jaut_ISerialisableAudioProcessor.h
    @date   08, February 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_audio/processor/jaut_ProcessSpec.h>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_data_structures/juce_data_structures.h>



//======================================================================================================================
namespace juce
{
    class Component;
}

//======================================================================================================================
namespace jaut
{
    /**
     *  A simple interface class for sub-audio processors for your own processor implementation.
     *  <br><br>
     *  In contrast to juce::AudioProcessor, this version of audio processor does not need the huge
     *  interface AudioProcessor provides.
     *  Instead, this class tries to reduce its interface to just the most important needs such as processing
     *  and, of course, serialising state.
     *  <br><br>
     *  If you want to use a AudioProcessor-based processor, you can proxy it with SerialisableJuceProcessorProxy.
     *  
     *  @see SerialisableJuceProcessorProxy
     */
    struct ISerialisableAudioProcessor
    {
        //==============================================================================================================
        virtual ~ISerialisableAudioProcessor() = default;
        
        //==============================================================================================================
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
        
        //==============================================================================================================
        /**
         *  Gets the name of this processor object.
         *  @return The processor name
         */
        JAUT_NODISCARD
        virtual juce::String getName() const = 0;
    
        //==============================================================================================================
        /**
         *  If this processor creates a component, it can be embedded by overriding this method.
         *  @return The new component
         */
        JAUT_NODISCARD
        virtual std::unique_ptr<juce::Component> createComponent() const { return nullptr; }
        
        //==============================================================================================================
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
        
        //==============================================================================================================
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
}
