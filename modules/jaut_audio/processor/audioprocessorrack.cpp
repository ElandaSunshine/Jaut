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
    @file   audioprocessorrack.cpp
    @date   29, December 2019
    
    ===============================================================
 */

//region Namespace
//======================================================================================================================
namespace
{
inline int indexOf(const jaut::AudioProcessorRack::ProcessorVector &rack, const juce::String &id)
{
    for (int i = 0; i < static_cast<int>(rack.size()); ++i)
    {
        const auto device = rack.at(static_cast<jaut::SizeTypes::Vector>(i));

        if (device->getName().trim().equalsIgnoreCase(id))
        {
            return i;
        }
    }

    return -1;
}
}
//======================================================================================================================
//endregion Namespace

namespace jaut
{
//**********************************************************************************************************************
// region UndoHistory
//======================================================================================================================
class AudioProcessorRack::UndoableAdd final : public juce::UndoableAction
{
public:
    UndoableAdd(AudioProcessorRack *rack, juce::String itemId)
        : rack(rack), itemId(std::move(itemId)), data("Data")
    {}

    //==================================================================================================================
    bool perform() override
    {
        if (rack)
        {
            juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
            auto &source_vec = rack->source;
            
            if (auto new_processor = rack->initialisationCallback(itemId))
            {
                (void) source_vec.emplace_back(new_processor);
                new_processor->prepare(rack->processSpec);
                new_processor->readData(data);
                
                rack->send<MessageSwap>(source_vec);
                return true;
            }
        }
        
        return false;
    }

    bool undo() override
    {
        if (rack)
        {
            juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
            auto &source_vec = rack->source;
    
            source_vec.back()->writeData(data);
            source_vec.pop_back();
            
            rack->send<MessageSwap>(source_vec);
            return true;
        }
        
        return false;
    }

    //==================================================================================================================
    int getSizeInUnits() override
    {
        return static_cast<int>(sizeof(*this));
    }

private:
    AudioProcessorRack *rack;
    juce::String itemId;
    juce::ValueTree data;
};

class AudioProcessorRack::UndoableRemove final : public juce::UndoableAction
{
public:
    UndoableRemove(AudioProcessorRack *rack, int itemIndex, juce::String itemId)
        : rack(rack), data("Data"), itemId(std::move(itemId)),
          itemIndex(itemIndex)
    {}

    //==================================================================================================================
    bool perform() override
    {
        if (rack)
        {
            juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
            auto &source_vec = rack->source;
            
            auto it = source_vec.begin() + itemIndex;
            it->get()->writeData(data);
            (void) source_vec.erase(it);
            rack->send<MessageSwap>(source_vec);
            
            return true;
        }
        
        return false;
    }
    
    bool undo() override
    {
        if (rack)
        {
            juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
            auto &source_vec = rack->source;
            
            if (auto new_processor = rack->initialisationCallback(itemId))
            {
                new_processor->readData(data);
                new_processor->prepare(rack->processSpec);
                
                (void) rack->source.insert(source_vec.begin() + itemIndex, new_processor);
                rack->send<MessageSwap>(source_vec);
                
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
    juce::ValueTree data;
    juce::String itemId;
    int itemIndex;
};

class AudioProcessorRack::UndoableClear final : public juce::UndoableAction
{
public:
    explicit UndoableClear(AudioProcessorRack *rack)
        : rack(rack), data("Data")
    {}

    //==================================================================================================================
    bool perform() override
    {
        if (rack)
        {
            juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
    
            if (!rack->source.empty())
            {
                rack->writeData(data);
                rack->send<MessageSwap>(rack->source);
            }
            
            return true;
        }
        
        return false;
    }

    bool undo() override
    {
        if (rack)
        {
            juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
            rack->readData(data);
            rack->send<MessageSwap>(rack->source);
            
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
    juce::ValueTree data;
};

class AudioProcessorRack::UndoableMove final : public juce::UndoableAction
{
public:
    UndoableMove(AudioProcessorRack *rack, int from, int to)
        : rack(rack), newIndex(to), oldIndex(from)
    {}

    //==================================================================================================================
    bool perform() override
    {
        if (rack)
        {
            juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
            
            auto &source_vec = rack->source;
            makeRotation(oldIndex, newIndex);
            rack->send<MessageSwap>(source_vec);
            
            return true;
        }
        
        return false;
    }

    bool undo() override
    {
        if (rack)
        {
            juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
            
            auto &source_vec = rack->source;
            makeRotation(newIndex, oldIndex);
            rack->send<MessageSwap>(source_vec);

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
    int newIndex;
    int oldIndex;

    //==================================================================================================================
    void makeRotation(int indexToMove, int targetIndex)
    {
        ProcessorVector &vector = rack->source;

        if (targetIndex < 0)
        {
            targetIndex = 0;
        }
        else if (targetIndex >= static_cast<int>(vector.size()))
        {
            targetIndex = static_cast<int>(vector.size() - 1);
        }
        
        if (indexToMove < targetIndex)
        {
            for (int i = indexToMove; i < targetIndex; ++i)
            {
                std::swap(vector.at(static_cast<SizeTypes::Vector>(i)),
                          vector.at(static_cast<SizeTypes::Vector>(i + 1)));
            }
        }
        else
        {
            for (int i = indexToMove; i > targetIndex; --i)
            {
                std::swap(vector.at(static_cast<SizeTypes::Vector>(i)),
                          vector.at(static_cast<SizeTypes::Vector>(i - 1)));
            }
        }
    }
};
//======================================================================================================================
// endregion UndoHistory
//**********************************************************************************************************************
// region Messages
//======================================================================================================================
class AudioProcessorRack::MessageSwap final : public IMessage
{
public:
    explicit MessageSwap(ProcessorVector source) noexcept
        : sourceCopy(std::move(source))
    {}

    //==================================================================================================================
    void handleMessage(IMessageHandler *context, MessageDirection messageDirection) override
    {
        AudioProcessorRack &rack = *static_cast<AudioProcessorRack*>(context);
        std::swap(sourceCopy, rack.devices);
    }

private:
    ProcessorVector sourceCopy;
};
//======================================================================================================================
// endregion Messages
//**********************************************************************************************************************
// region AudioProcessorRack
//======================================================================================================================
AudioProcessorRack::AudioProcessorRack(InitCallback initCallback, juce::UndoManager *undoManager) noexcept
    : initialisationCallback(std::move(initCallback)), undoManager(undoManager)
{}

//======================================================================================================================
bool AudioProcessorRack::addProcessor(const juce::String &processorId)
{
    const juce::String id = processorId.trim().toLowerCase();
    return undoManager ? undoManager->perform(new UndoableAdd(this, id)) : UndoableAdd(this, id).perform();
}

bool AudioProcessorRack::removeProcessor(int index)
{
    const int adjusted_index = std::clamp<int>(index, 0, source.size() - 1);
    const juce::String id    = source.at(static_cast<SizeTypes::Vector>(adjusted_index))->getName()
                                     .trim().toLowerCase();
    
    return undoManager ? undoManager->perform(new UndoableRemove(this, adjusted_index, id))
                       : UndoableRemove(this, adjusted_index, id).perform();
}

bool AudioProcessorRack::moveProcessor(int from, int to)
{
    const int vec_end       = getNumProcessors() - 1;
    const int adjusted_from = std::clamp<int>(from, 0, vec_end);
    const int adjusted_to   = std::clamp<int>(to,   0, vec_end);
    
    return undoManager ? undoManager->perform(new UndoableMove(this, adjusted_from, adjusted_to))
                       : UndoableMove(this, adjusted_from, adjusted_to).perform();
}

bool AudioProcessorRack::clear()
{
    return undoManager ? undoManager->perform(new UndoableClear(this)) : UndoableClear(this).perform();
}

//======================================================================================================================
int AudioProcessorRack::getNumProcessors() const noexcept
{
    return source.size();
}

//==================================================================================================================
AudioProcessorRack::Processor* AudioProcessorRack::getProcessor(int index) noexcept
{
    return fit(index, 0, getNumProcessors()) ? source.at(static_cast<SizeTypes::Vector>(index)).get() : nullptr;
}
const AudioProcessorRack::Processor* AudioProcessorRack::getProcessor(int index) const noexcept
{
    return jaut::fit(index, 0, getNumProcessors()) ? source.at(static_cast<SizeTypes::Vector>(index)).get() : nullptr;
}

//======================================================================================================================
void AudioProcessorRack::process(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiBuffer)
{
    processNextMessage();
    
    for (auto &device : devices)
    {
        device->process(buffer, midiBuffer);
    }
}

void AudioProcessorRack::process(juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiBuffer)
{
    processNextMessage();

    for (auto &device : devices)
    {
        device->process(buffer, midiBuffer);
    }
}

void AudioProcessorRack::prepare(ProcessSpec spec)
{
    for (auto &device : devices)
    {
        device->prepare(spec);
    }
    
    std::swap(processSpec, spec);
}

void AudioProcessorRack::release()
{
    for (auto &device : devices)
    {
        device->release();
    }
}

//======================================================================================================================
void AudioProcessorRack::readData(const juce::ValueTree data)
{
    ProcessorVector data_vector;
    data_vector.reserve(static_cast<SizeTypes::Vector>(data.getNumChildren()));
    
    for (const auto &child : data)
    {
        if (child.hasType("RackDevice"))
        {
            const juce::String id = child.getProperty("Id", "");

            if (auto device = initialisationCallback(id))
            {
                if (juce::ValueTree device_data = child.getChild(0); device_data.isValid())
                {
                    device->readData(device_data);
                }
                
                data_vector.emplace_back(device);
            }
        }
    }
    
    data_vector.shrink_to_fit();
    
    if (juce::MessageManager::getInstance()->isThisTheMessageThread())
    {
        std::swap(source, data_vector);
        send<MessageSwap>(source);
    }
    else
    {
        if (!juce::MessageManager::callAsync
             (
                 [this, data_vector]() mutable
                 {
                     std::swap(source, data_vector);
                     send<MessageSwap>(source);
                 }
             ))
        {
            juce::CriticalSection::ScopedLockType lock(sourceLock);
            std::swap(source, data_vector);
    
            {
                ThreadexSwitch threadex;
                send<MessageSwap>(source);
            }
        }
    }
}

void AudioProcessorRack::writeData(juce::ValueTree data)
{
    ProcessorVector data_vector;
    
    if (juce::MessageManager::getInstance()->isThisTheMessageThread())
    {
        data_vector = source;
    }
    else
    {
        juce::CriticalSection::ScopedLockType lock(sourceLock);
        data_vector = source;
    }
    
    if (data.isValid())
    {
        data.removeAllChildren(nullptr);
        
        for (auto &device : data_vector)
        {
            const juce::String id = device->getName().removeCharacters(" ").toLowerCase();
    
            juce::ValueTree device_state("RackDevice");
            (void) device_state.setProperty("Id", id, nullptr);
    
            juce::ValueTree device_data("Data");
            device->writeData(device_data);
            device_state.appendChild(device_data, nullptr);

            data.appendChild(device_state, nullptr);
        }
    }
}
//======================================================================================================================
//endregion AudioProcessorRack
//**********************************************************************************************************************
}
