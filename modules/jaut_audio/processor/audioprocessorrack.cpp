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
    @file   audioprocessorrack.cpp
    @date   29, December 2019
    
    ===============================================================
 */

#include "audioprocessorrack.h"

using namespace juce;

//region Namespace
/* ================================================================================================================== */
namespace
{
inline int indexOf(const jaut::AudioProcessorRack::ProcessorVector &rack, const String &id)
{
    for(int i = 0; i < rack.size(); ++i)
    {
        const auto device = rack.at(i);

        if(device->getName().removeCharacters(" ").toLowerCase() == id)
        {
            return i;
        }
    }

    return -1;
}
}
//endregion Namespace

namespace jaut
{
//region UndoHistory
/* ================================================================================================================== */
class AudioProcessorRack::UndoableAdd final : public UndoableAction
{
public:
    UndoableAdd(AudioProcessorRack *rack, String itemId)
        : rack(rack), itemId(std::move(itemId)),
          previousIndex(-1)
    {}

    //==================================================================================================================
    bool perform() override
    {
        if(rack)
        {
            CriticalSection::ScopedLockType lock(rack->sourceLock);
            auto &source_vec = rack->source;

            if(::indexOf(source_vec, itemId) == -1)
            {
                if(auto new_processor = rack->initialisationCallback(itemId))
                {
                    // In case we previously undid adding a processor, we redo adding it with the previous state data.
                    if(data.isValid())
                    {
                        new_processor->readData(data);
                    }
                    
                    new_processor->prepareToPlay(rack->getSampleRate(), rack->getBlockSize());
                    new_processor->setPlayHead(rack->getPlayHead());
                    source_vec.emplace_back(new_processor);
    
                    if(previousIndex >= 0)
                    {
                        rack->sourceIndex = std::exchange(previousIndex, -1);
                    }
                    
                    rack->send<MessageSwap>(source_vec, rack->sourceIndex);
                    
                    return true;
                }
            }
        }

        return false;
    }

    bool undo() override
    {
        if(rack)
        {
            CriticalSection::ScopedLockType lock(rack->sourceLock);
            auto &source_vec          = rack->source;
            const int processor_index = ::indexOf(source_vec, itemId);

            if(processor_index >= 0)
            {
                data = ValueTree("Data");
                source_vec.at(processor_index)->writeData(data); // save state in case we redo the undo
                previousIndex = rack->sourceIndex;
    
                source_vec.erase(rack->source.begin() + processor_index);
                
                if(rack->sourceIndex >= source_vec.size())
                {
                    rack->sourceIndex = static_cast<int>(source_vec.size() - 1);
                }
                
                rack->send<MessageSwap>(source_vec, rack->sourceIndex);
                
                return true;
            }
        }
        
        return false;
    }

    //==================================================================================================================
    int getSizeInUnits()  override
    {
        return static_cast<int>(sizeof(*this));
    }

private:
    AudioProcessorRack *rack;
    String itemId;
    ValueTree data;
    int previousIndex;
};

class AudioProcessorRack::UndoableRemove final : public UndoableAction
{
public:
    UndoableRemove(AudioProcessorRack *rack, String itemId)
        : rack(rack), itemId(std::move(itemId)), data("Data"),
          previousIndex(-1)
    {}

    //==================================================================================================================
    bool perform() override
    {
        if(rack)
        {
            CriticalSection::ScopedLockType lock(rack->sourceLock);
            auto &source_vec          = rack->source;
            const int processor_index = ::indexOf(source_vec, itemId);
            
            if(processor_index >= 0)
            {
                source_vec.at(processor_index)->writeData(data);
                previousIndex = rack->sourceIndex;
                
                source_vec.erase(source_vec.begin() + processor_index);
                
                if(rack->sourceIndex >= source_vec.size())
                {
                    rack->sourceIndex = static_cast<int>(source_vec.size() - 1);
                }
                
                rack->send<MessageSwap>(source_vec, rack->sourceIndex);
                
                return true;
            }
        }
        
        return false;
    }
    
    bool undo() override
    {
        if(rack)
        {
            CriticalSection::ScopedLockType lock(rack->sourceLock);
            auto &source_vec = rack->source;
            
            if(::indexOf(source_vec, itemId) == -1)
            {
                if(auto new_processor = rack->initialisationCallback(itemId))
                {
                    new_processor->readData(data);
                    new_processor->prepareToPlay(rack->getSampleRate(), rack->getBlockSize());
                    new_processor->setPlayHead(rack->getPlayHead());
                    rack->source.emplace_back(new_processor);
                    
                    if(previousIndex >= 0)
                    {
                        rack->sourceIndex = std::exchange(previousIndex, -1);
                    }
                    
                    rack->send<MessageSwap>(source_vec, rack->sourceIndex);
                    
                    return true;
                }
            }
        }
        
        return false;
    }
    
    //==================================================================================================================
    int getSizeInUnits()  override
    {
        return static_cast<int>(sizeof(*this));
    }

private:
    AudioProcessorRack *rack;
    String itemId;
    ValueTree data;
    int previousIndex;
};

class AudioProcessorRack::UndoableClear final : public UndoableAction
{
public:
    explicit UndoableClear(AudioProcessorRack *rack)
        : rack(rack), previousIndex(-1)
    {}

    //==================================================================================================================
    bool perform() override
    {
        if(rack && !(rack->source.empty()))
        {
            CriticalSection::ScopedLockType lock(rack->sourceLock);
    
            data          = ValueTree("Data");
            previousIndex = rack->sourceIndex;
            rack->writeData(data);
            
            rack->send<MessageSwap>(rack->source, (rack->sourceIndex = 0));
            
            return true;
        }
        
        return false;
    }

    bool undo() override
    {
        if(rack && data.isValid())
        {
            CriticalSection::ScopedLockType lock(rack->sourceLock);
            
            rack->readData(data);
            data = ValueTree();
            
            if(previousIndex >= 0)
            {
                rack->sourceIndex = std::exchange(previousIndex, -1);
            }
            
            rack->send<MessageSwap>(rack->source, rack->sourceIndex);
            
            return true;
        }

        return false;
    }

    //==================================================================================================================
    int getSizeInUnits()  override
    {
        return static_cast<int>(sizeof(*this));
    }

private:
    AudioProcessorRack *rack;
    ValueTree data;
    int previousIndex;
};

class AudioProcessorRack::UndoableMove final : public UndoableAction
{
public:
    UndoableMove(AudioProcessorRack *rack, String id, int index)
        : rack(rack), itemId(std::move(id)), itemIndex(index)
    {}

    //==================================================================================================================
    bool perform() override
    {
        if (rack)
        {
            CriticalSection::ScopedLockType lock(rack->sourceLock);
            
            auto &source_vec          = rack->source;
            const int processor_index = ::indexOf(source_vec, itemId);
            
            if (processor_index >= 0 && processor_index != itemIndex)
            {
                makeRotation(processor_index, itemIndex);
                
                if (processor_index == rack->sourceIndex)
                {
                    rack->sourceIndex = itemIndex;
                }
                
                itemIndex = processor_index;
                rack->send<MessageSwap>(source_vec, rack->sourceIndex);
                
                return true;
            }
        }
        
        return false;
    }

    bool undo() override
    {
        if (rack)
        {
            CriticalSection::ScopedLockType lock(rack->sourceLock);
            
            auto &source_vec          = rack->source;
            const int processor_index = ::indexOf(source_vec, itemId);

            makeRotation(processor_index, itemIndex);
    
            if (processor_index == rack->sourceIndex)
            {
                rack->sourceIndex = itemIndex;
            }
            
            itemIndex = processor_index;
            rack->send<MessageSwap>(source_vec, rack->sourceIndex);

            return true;
        }

        return false;
    }

    //==================================================================================================================
    int getSizeInUnits()  override
    {
        return static_cast<int>(sizeof(*this));
    }

private:
    AudioProcessorRack *rack;
    String itemId;
    int itemIndex;

    //==================================================================================================================
    void makeRotation(int indexToMove, int targetIndex)
    {
        ProcessorVector &vector = rack->source;

        if(targetIndex < 0)
        {
            targetIndex = 0;
        }
        else if(targetIndex >= vector.size())
        {
            targetIndex = static_cast<int>(vector.size() - 1);
        }
        
        if(indexToMove < targetIndex)
        {
            for(int i = indexToMove; i < targetIndex; ++i)
            {
                std::swap(vector.at(i), vector.at(i + 1));
            }
        }
        else
        {
            for(int i = indexToMove; i > targetIndex; --i)
            {
                std::swap(vector.at(i), vector.at(i - 1));
            }
        }
    }
};

class AudioProcessorRack::UndoableSelect final : public UndoableAction
{
public:
    UndoableSelect(AudioProcessorRack *rack, int index)
        : rack(rack), itemIndex(index)
    {}
    
    //==================================================================================================================
    bool perform() override
    {
        if(!rack)
        {
            CriticalSection::ScopedLockType lock(rack->sourceLock);
    
            if(jaut::fit_s<int>(itemIndex, 0, rack->source.size()) && itemIndex != rack->sourceIndex)
            {
                rack->sendToAll<MessageSelect>(std::exchange(itemIndex, rack->sourceIndex));
                return true;
            }
        }
    
        return false;
    }
    
    bool undo() override
    {
        return perform();
    }
    
    //==================================================================================================================
    int getSizeInUnits()  override
    {
        return static_cast<int>(sizeof(*this));
    }

private:
    AudioProcessorRack *rack;
    int itemIndex;
};
//endregion UndoHistory
//region Messages
/* ================================================================================================================== */
class AudioProcessorRack::MessageSwap final : public IMessage
{
public:
    MessageSwap(ProcessorVector source, int index)
        : sourceCopy(std::move(source)),
          index(index)
    {}

    //==================================================================================================================
    void handleMessage(IMessageHandler *context, MessageDirection messageDirection) override
    {
        AudioProcessorRack &rack = *static_cast<AudioProcessorRack*>(context); // NOLINT
        std::swap(sourceCopy, rack.devices);
        rack.deviceIndex = index;
    }

private:
    ProcessorVector sourceCopy;
    int index;
};

class AudioProcessorRack::MessageSelect final : public IMessage
{
public:
    explicit MessageSelect(int newIndex)
        : index(newIndex)
    {}
    
    //==================================================================================================================
    void handleMessage(IMessageHandler *context, MessageDirection messageDirection) override
    {
        AudioProcessorRack &rack = *static_cast<AudioProcessorRack*>(context); // NOLINT
        
        if(messageDirection == MessageThread)
        {
            rack.sourceIndex = index;
        }
        else
        {
            rack.deviceIndex = index;
        }
    }

private:
    int index;
};
//endregion Messages
//region AudioProcessorRack
/* ==================================================================================
 * =============================== AudioProcessorRack ===============================
 * ================================================================================== */
AudioProcessorRack::AudioProcessorRack(ProcessingMode mode, InitCallback initCallback,
                                       UndoManager *undoManager) noexcept
    : initialisationCallback(std::move(initCallback)), mode(mode),
      undoManager(undoManager), deviceIndex(0), sourceIndex(0)
{
    JAUT_ENSURE_MESSAGE_THREAD;
}

//======================================================================================================================
bool AudioProcessorRack::addProcessor(const String &processorId)
{
    JAUT_ENSURE_MESSAGE_THREAD;
    const String id = processorId.removeCharacters(" ").trim();
    return undoManager ? undoManager->perform(new UndoableAdd(this, id)) : UndoableAdd(this, id).perform();
}

bool AudioProcessorRack::removeProcessor(const String &processorId)
{
    JAUT_ENSURE_MESSAGE_THREAD;
    const String id = processorId.removeCharacters(" ").trim();
    return undoManager ? undoManager->perform(new UndoableRemove(this, id)) : UndoableRemove(this, id).perform();
}

bool AudioProcessorRack::moveProcessor(const String &processorId, int index)
{
    JAUT_ENSURE_MESSAGE_THREAD;
    const String id = processorId.removeCharacters(" ").trim();
    return undoManager ? undoManager->perform(new UndoableMove(this, id, index)) :
                         UndoableMove(this, id, index).perform();
}

bool AudioProcessorRack::clear()
{
    JAUT_ENSURE_MESSAGE_THREAD;
    return undoManager ? undoManager->perform(new UndoableClear(this)) : UndoableClear(this).perform();
}

//======================================================================================================================
const String AudioProcessorRack::getName() const // NOLINT
{
    return "AudioProcessorRack";
}

int AudioProcessorRack::getNumProcessors() const noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD;
    return source.size();
}

//==================================================================================================================
AudioProcessorRack::Processor* AudioProcessorRack::getProcessor(int index) noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD;
    return jaut::fit_s(index, 0, getNumProcessors()) ? source.at(index).get() : nullptr;
}

AudioProcessorRack::Processor* AudioProcessorRack::getProcessor(const String &processorId) noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD;

    const String id = processorId.removeCharacters(" ").toLowerCase();
    const int index = ::indexOf(source, id);
    return index >= 0 ? source.at(index).get() : nullptr;
}

const AudioProcessorRack::Processor* AudioProcessorRack::getProcessor(int index) const noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD;
    return jaut::fit_s(index, 0, getNumProcessors()) ? source.at(index).get() : nullptr;
}

const AudioProcessorRack::Processor* AudioProcessorRack::getProcessor(const String &processorId) const noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD;
    
    const String id = processorId.removeCharacters(" ").toLowerCase();
    const int index = ::indexOf(source, id);
    return index >= 0 ? source.at(index).get() : nullptr;
}

//======================================================================================================================
AudioProcessorRack::Processor* AudioProcessorRack::getActivated()
{
    JAUT_ENSURE_MESSAGE_THREAD;
    return mode == Single && !source.empty() ? source.at(getActiveIndex()).get() : nullptr;
}

String AudioProcessorRack::getActivatedId() const
{
    JAUT_ENSURE_MESSAGE_THREAD;
    return mode == Single ? source.at(getActiveIndex())->getName().removeCharacters(" ").toLowerCase() : String();
}

int AudioProcessorRack::getActiveIndex() const
{
    JAUT_ENSURE_MESSAGE_THREAD;
    return sourceIndex;
}

bool AudioProcessorRack::setActivated(int index)
{
    JAUT_ENSURE_MESSAGE_THREAD;
    
    if(mode == Serial)
    {
        return false;
    }
    
    if(index != sourceIndex && jaut::fit_s<int>(index, 0, source.size()))
    {
        sendToAll<MessageSelect>(index);
        return true;
    }

    return false;
}

bool AudioProcessorRack::setActivated(const String &processorId)
{
    JAUT_ENSURE_MESSAGE_THREAD;
    const String id = processorId.removeCharacters(" ").toLowerCase();
    const int index = ::indexOf(source, id);
    return setActivated(index);
}

//======================================================================================================================
void AudioProcessorRack::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer)
{
    processNextMessage();

    // Process
    if(mode == Serial)
    {
        for(auto &device : devices)
        {
            device->processBlock(buffer, midiBuffer);
        }
    }
    else
    {
        if(devices.empty())
        {
            return;
        }

        devices.at(sourceIndex)->processBlock(buffer, midiBuffer);
    }
}

void AudioProcessorRack::processBlock(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer)
{
    processNextMessage();

    // Process
    if(mode == Serial)
    {
        for(auto &device : devices)
        {
            device->processBlock(buffer, midiBuffer);
        }
    }
    else
    {
        if(devices.empty())
        {
            return;
        }

        devices.at(sourceIndex)->processBlock(buffer, midiBuffer);
    }
}

void AudioProcessorRack::prepareToPlay(double sampleRate, int maxBlockSamples)
{
    setRateAndBufferSizeDetails(sampleRate, maxBlockSamples);

    for (auto &device : devices)
    {
        device->setRateAndBufferSizeDetails(sampleRate, maxBlockSamples);
        device->prepareToPlay(sampleRate, maxBlockSamples);
    }
}

void AudioProcessorRack::releaseResources()
{
    for (auto &device : devices)
    {
        device->releaseResources();
    }
}

//======================================================================================================================
void AudioProcessorRack::readData(const ValueTree data)
{
    ProcessorVector data_vector;
    const int selected_index = data.getProperty("ActiveIndex", 0);
    
    data_vector.reserve(data.getNumChildren());

    for(const auto &child : data)
    {
        if(child.hasType("RackDevice"))
        {
            const String id = child.getProperty("Id", "");

            if(auto device = initialisationCallback(id))
            {
                ValueTree device_data = child.getChild(0);
                
                if(device_data.isValid())
                {
                    device->readData(device_data);
                }
                
                device->setPlayHead(getPlayHead());
                data_vector.emplace_back(device);
            }
        }
    }
    
    data_vector.shrink_to_fit();
    
    if(MessageManager::getInstance()->isThisTheMessageThread())
    {
        std::swap(source, data_vector);
        send<MessageSwap>(source, selected_index);
    }
    else
    {
        if (!MessageManager::callAsync([this, data_vector, selected_index]() mutable {std::swap(source, data_vector);}))
        {
            CriticalSection::ScopedLockType lock(sourceLock);
            std::swap(source, data_vector);
        }
        
        {
            CriticalSection::ScopedLockType lock(getCallbackLock());
            std::swap(devices, data_vector);
        }
    }
}

void AudioProcessorRack::writeData(ValueTree data)
{
    ProcessorVector data_vector(source);

    if(data.isValid())
    {
        data.removeAllChildren(nullptr);
        data.setProperty("ActiveIndex", getActiveIndex(), nullptr);

        for(auto &device : data_vector)
        {
            const String id = device->getName().removeCharacters(" ").toLowerCase();

            ValueTree device_state("RackDevice");
            (void) device_state.setProperty("Id", id, nullptr);

            ValueTree device_data("Data");
            device->writeData(device_data);
            device_state.appendChild(device_data, nullptr);

            data.appendChild(device_state, nullptr);
        }
    }
}
//endregion AudioProcessorRack
}
