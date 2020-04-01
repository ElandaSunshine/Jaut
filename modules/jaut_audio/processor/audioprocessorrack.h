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
    @file   audioprocessorrack.h
    @date   27, December 2019
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  The rack processor keeps an internal collection of AudioProcessor objects and plays them either in
 *  series or as single processors.
 *
 *  This processor is a lock-free implementation, by that means, in each processCallback it will check
 *  any pending update messages from the message thread and handle them one at a time.
 *
 *  Processors are stored and differentiated by IDs, these IDs are represented as a case-insensitive (they will be
 *  stored lowercase anyway) and whitespaceless string.
 *  Each ID must be unique and can't be assigned twice.
 *
 *  The initialisation function that is passed through the constructor of this processor, handles the mapping of
 *  IDs to processors.
 *  Whenever there needs to be a new processor added, it will call this function and return the processor which is
 *  assigned the id.
 *  Note that processor must be dynamically allocated.
 *
 *  Important for IDs is that they can't contain any whitespaces in the middle of the string, whitespaces at the
 *  end and beginning will be trimmed.
 *  In addition, all IDs will be converted to lower-case, so don't expect mapping different IDs only through
 *  changing its case representation or adding whitespaces.
 *
 *  Such a function may look like:
 *  @code
 *  std::shared_ptr<SerializableAudioProcessor> createProcessorFromId(const juce::String &id)
 *  {
 *      if(id == "processor1")
 *      {
 *          return std::make_shared<AudioProcessor1>();
 *      }
 *      else if(id == "processor2")
 *      {
 *          return std::make_shared<AudioProcessor2>();
 *      }
 *
 *      return nullptr;
 *  }
 *  @endcode
 */
class JAUT_API AudioProcessorRack final : public SerializableAudioProcessor, public MessageHandler<16>
{
public:
    /**
     *  The processing mode determines whether to play all processors in sequence or if only one should
     *  be played which is determined by index.
     */
    enum JAUT_API ProcessingMode
    {
        /**
         *  Play all processor in series
         */
        Serial,
        
        /**
         *  Play only the selected processor
         */
        Single
    };

    //==================================================================================================================
    /**
     *  The type of the child processors
     */
    using Processor = SerializableAudioProcessor;
    
    /**
     *  The type of the pointer of the type of the child processors
     */
    using ProcessorPointer = std::shared_ptr<Processor>;
    
    /**
     *  The type of the vector containing the type of the pointer of the type of the child processors
     */
    using ProcessorVector = std::vector<ProcessorPointer>;
    
    /**
     *  The type of function to initialize the processors with
     */
    using InitCallback = std::function<ProcessorPointer(const juce::String&)>;

    //==================================================================================================================
    /**
     *  Creates a new AudioProcessorRack instance.
     *
     *  @param mode         The processing mode of this processor
     *  @param initCallback The child processor initialisation function
     *  @param undoManager  The UndoManager to use or nullptr if non should be used
     */
    AudioProcessorRack(ProcessingMode mode, InitCallback initCallback,
                       juce::UndoManager *undoManager = nullptr) noexcept;

    //==================================================================================================================
    /**
     *  Calls the initialisation function and creates the new processors if the id was defined.
     *  If no such id was defined or the rack already has an instance of the processor, then this will do nothing
     *  and return false.
     *
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @param processorId The id of the processor to initialise and add (case-insensitive)
     *  @return True if the processor was successfully added, false if not
     */
    bool addProcessor(const juce::String &processorId);
    
    /**
     *  Removes a processor from the rack if there was one with this id, else will do nothing and return false.
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @param processorId The id of the processor to remove (case-insensitive)
     *  @return True if the processor was successfully removed, false if not
     */
    bool removeProcessor(const juce::String &processorId);
    
    /**
     *  Tries to move a processor from one index to another if this id was found.
     *  If the processor is already on the result index or if there is no such processor, this will do nothing and
     *  return false.
     *
     *  If the index is bigger than the rack size, this device will be moved to the end and if it is smaller than
     *  zero it will be moved to the beginning.
     *
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @param processorId The processor to move (case-insensitive)
     *  @param index       The index to move the processor to
     *  @return True if the processor was moved, false if not
     */
    bool moveProcessor(const juce::String &processorId, int index);
    
    /**
     *  Clears all processors from the rack.
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @return True if there were any processors in the rack, false if it was empty
     */
    bool clear();

    //==================================================================================================================
    const juce::String getName() const override;
    bool hasEditor() const override { return false; }

    //==================================================================================================================
    /**
     *  Gets the amount of processors in the rack.
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @return The amount of processors
     */
    int getNumProcessors() const noexcept;

    //==================================================================================================================
    /**
     *  Gets a processor at a specified index.
     *  If the index is out of bounds, this will return a nullptr.
     *
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @param index The index of the processor
     *  @return The processor at the specified index, or nullptr if index was out of bounds
     */
    Processor* getProcessor(int index) noexcept;
    
    /**
     *  Gets a processor by its id.
     *  If the id is not found, this will return a nullptr.
     *
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @param processorId The id of the processor
     *  @return The processor with the specified id, or nullptr if there was no such id
     */
    Processor* getProcessor(const juce::String &processorId) noexcept;
    
    /**
     *  Gets a processor at a specified index.
     *  If the index is out of bounds, this will return a nullptr.
     *
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @param index The index of the processor
     *  @return The processor at the specified index, or nullptr if index was out of bounds
     */
    const Processor* getProcessor(int index) const noexcept;
    
    /**
     *  Gets a processor by its id.
     *  If the id is not found, this will return a nullptr.
     *
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @param processorId The id of the processor
     *  @return The processor with the specified id, or nullptr if there was no such id
     */
    const Processor* getProcessor(const juce::String &processorId) const noexcept;

    //==================================================================================================================
    /**
     *  Gets the current selected processor.
     *  If set to serial mode or the rack is empty, this will return nullptr.
     *
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @return The processor
     */
    Processor* getActivated();
    
    /**
     *  Gets the id of the current selected processor.
     *  If set to serial mode or the rack is empty, this will return an empty String.
     *
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @return The id of the processor
     */
    juce::String getActivatedId() const;
    
    /**
     *  Gets the index of the current selected processor.
     *  If set to serial mode or the rack is empty, this will return 0.
     *
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @return The index of the processor
     */
    int getActiveIndex() const;
    
    /**
     *  In single mode, this will change the current selected processor.
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @param index The new index
     *  @return True if a new processor was selected, false if not
     */
    bool setActivated(int index);
    
    /**
     *  In single mode, this will change the current selected processor.
     *  Note that this should only be called on the message thread. (or disable threadex)
     *
     *  @param processorId The id of the processor that should be selected
     *  @return True if a new processor was selected, false if not
     */
    bool setActivated(const juce::String &processorId);

    //==================================================================================================================
    void processBlock(juce::AudioBuffer<float> &buffer,  juce::MidiBuffer &midiBuffer) override;
    void processBlock(juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiBuffer) override;
    void prepareToPlay(double sampleRate, int maxBlockSamples) override;
    void releaseResources() override;

    //==================================================================================================================
    void readData(const juce::ValueTree data) override;
    void writeData(juce::ValueTree data) override;

private:
    class UndoableAdd;
    class UndoableRemove;
    class UndoableClear;
    class UndoableMove;
    class UndoableSelect;
    
    class MessageSwap;
    class MessageSelect;
    
    //==================================================================================================================
    InitCallback initialisationCallback;
    ProcessingMode mode;
    juce::UndoManager *undoManager;
    juce::CriticalSection sourceLock;
    
    ProcessorVector devices, source;
    int deviceIndex, sourceIndex;
};
}
