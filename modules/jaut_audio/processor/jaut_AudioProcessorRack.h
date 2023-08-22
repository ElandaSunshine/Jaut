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
    @file   jaut_AudioProcessorRack.h
    @date   27, December 2019
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_audio/processor/jaut_SerialisableJuceProcessorProxy.h>
#include <jaut_message/thread/jaut_MessageHandler.h>

#include <juce_data_structures/juce_data_structures.h>



namespace jaut
{
    /**
     *  The rack processor keeps an internal collection of AudioProcessor objects and plays them either in
     *  series or as single processors.
     *  <br><br>
     *  This processor is a lock-free implementation, by that means, in each processCallback it will check
     *  any pending update messages from the message thread and handle them one at a time.
     *  <br><br>
     *  Processors are stored and differentiated by IDs, these IDs are represented as a case-insensitive (they will be
     *  stored lowercase anyway) and whitespace-less string.
     *  Each ID must be unique and can't be assigned twice.
     *  <br><br>
     *  The initialisation function that is passed through the constructor of this processor, handles the mapping of
     *  IDs to processors.
     *  Whenever there needs to be a new processor added, it will call this function and return the processor which is
     *  assigned the id.
     *  Note that processor must be dynamically allocated.
     *  <br><br>
     *  Important for IDs is that they can't contain any whitespaces in the middle of the string, whitespaces at the
     *  end and beginning will be trimmed.
     *  In addition, all IDs will be converted to lower-case, so don't expect mapping different IDs only through
     *  changing its case representation or adding whitespaces.
     *  <br><br>
     *  Such a function may look like:<br>
     *  @code{.cpp}
     *  std::unique_ptr<SerializableAudioProcessor> createProcessorFromId(const juce::String &id)
     *  {
     *      if (id == "processor1")
     *      {
     *          return std::make_shared<AudioProcessor1>();
     *      }
     *      else if (id == "processor2")
     *      {
     *          return std::make_shared<AudioProcessor2>();
     *      }
     *
     *      return nullptr;
     *  }
     *  @endcode
     */
    class JAUT_API AudioProcessorRack : public ISerialisableAudioProcessor, public MessageHandler<16>
    {
    public:
        /** The type of the child processors. */
        using Processor = ISerialisableAudioProcessor;
        
        /** The type of the pointer of the type of the child processors. */
        using ProcessorPointer = std::shared_ptr<Processor>;
        
        /** The type of the vector containing the type of the pointer of the type of the child processors. */
        using ProcessorVector = std::vector<ProcessorPointer>;
        
        /** The type of function to initialize the processors with. */
        using InitCallback = std::function<ProcessorPointer(const juce::String&)>;
    
        //==============================================================================================================
        /**
         *  Creates a new AudioProcessorRack instance.
         *
         *  @param initCallback The child processor initialisation function should be generated
         *  @param undoManager  The UndoManager to use or nullptr if non should be used
         */
        explicit AudioProcessorRack(InitCallback initCallback, juce::UndoManager *undoManager = nullptr) noexcept;
    
        //==============================================================================================================
        /**
         *  Calls the initialisation function and creates the new processors if the id was defined.
         *  If no such id was defined or the rack already has an instance of the processor, then this will do nothing
         *  and return false.
         *  <br><br>
         *  Note that this should only be called on the message thread.
         *
         *  @param processorId The id of the processor to initialise and add (case-insensitive)
         *  @return True if the processor was successfully added, false if not
         */
        bool addProcessor(const juce::String &processorId);
        
        /**
         *  Removes a processor from the rack if there was one with this index, else will do nothing and return false.
         *  Note that this should only be called on the message thread.
         *
         *  @param index The position of the processor to remove (case-insensitive)
         *  @return True if the processor was successfully removed, false if not
         */
        bool removeProcessor(int index);
        
        /**
         *  Tries to move a processor from one index to another if this id was found.
         *  If the processor is already on the result index or if there is no such processor, this will do nothing and
         *  return false.
         *  <br><br>
         *  If the index is bigger than the rack size, this device will be moved to the end and if it is smaller than
         *  zero it will be moved to the beginning.
         *  <br><br>
         *  Note that this should only be called on the message thread.
         *
         *  @param from The processor's old position
         *  @param to   The processor's new position
         *  @return True if the processor was moved, false if not
         */
        bool moveProcessor(int from, int to);
        
        /**
         *  Clears all processors from the rack.
         *  Note that this should only be called on the message thread.
         *
         *  @return The number of cleared processors
         */
        int clear();
    
        //==============================================================================================================
        JAUT_NODISCARD
        juce::String getName() const override;

        //==============================================================================================================
        /**
         *  Gets the amount of processors in the rack.
         *  Note that this should only be called on the message thread.
         *
         *  @return The amount of processors
         */
        JAUT_NODISCARD
        int getNumProcessors() const noexcept;

        //==============================================================================================================
        /**
         *  Gets a processor at a specified index.
         *  If the index is out of bounds, this will return a nullptr.
         *  <br><br>
         *  Note that this should only be called on the message thread.
         *
         *  @param index The index of the processor
         *  @return The processor at the specified index, or nullptr if index was out of bounds
         */
        JAUT_NODISCARD
        Processor* getProcessor(int index) noexcept;
        
        /**
         *  Gets a processor at a specified index.
         *  If the index is out of bounds, this will return a nullptr.
         *  <br><br>
         *  Note that this should only be called on the message thread.
         *
         *  @param index The index of the processor
         *  @return The processor at the specified index, or nullptr if index was out of bounds
         */
        JAUT_NODISCARD
        const Processor* getProcessor(int index) const noexcept;

        //==============================================================================================================
        void process(juce::AudioBuffer<float> &buffer,  juce::MidiBuffer &midiBuffer) override;
        void process(juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiBuffer) override;
        void prepare(ProcessSpec spec) override;
        void release() override;
        void reset() override;
        
        //==============================================================================================================
        void readData (juce::ValueTree data) override;
        void writeData(juce::ValueTree data) override;
        
    private:
        //==============================================================================================================
        class UndoableAdd;
        class UndoableRemove;
        class UndoableClear;
        class UndoableMove;
        class MessageSwap;
        
        //==============================================================================================================
        template<class Self>
        JAUT_NODISCARD
        static Processor* getProcessor_impl(Self &self, int index) noexcept;
        
        //==============================================================================================================
        ProcessorVector devices;
        ProcessorVector source;
        
        juce::CriticalSection sourceLock;
        InitCallback          initialisationCallback;
        juce::UndoManager     *undoManager;
        ProcessSpec           processSpec;
    
        //==============================================================================================================
        JUCE_DECLARE_NON_COPYABLE(AudioProcessorRack)
    };
    
    //==================================================================================================================
    // IMPLEMENTATION
    template<class Self>
    inline AudioProcessorRack::Processor* AudioProcessorRack::getProcessor_impl(Self &parSelf, int parIndex) noexcept
    {
        return ((parIndex >= 0 && parIndex < parSelf.source.size())? parSelf.source[parIndex].get() : nullptr);
    }
}
