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
    @file   jaut_AudioProcessorRack.cpp
    @date   29, December 2019
    
    ===============================================================
 */

#include <jaut_audio/processor/jaut_AudioProcessorRack.h>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    JAUT_NODISCARD
    juce::String prepareId(const juce::String &id)
    {
        return id.trim().toLowerCase();
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region UndoHistory
//======================================================================================================================
namespace jaut
{
    class AudioProcessorRack::UndoableAdd final : public juce::UndoableAction
    {
    public:
        UndoableAdd(AudioProcessorRack *const parRack, juce::String parItemId)
            : rack(parRack), itemId(std::move(parItemId)), data("Data")
        {}
        
        //==============================================================================================================
        bool perform() override
        {
            if (rack)
            {
                juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
                ProcessorVector &source_vec = rack->source;
                
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
                ProcessorVector &source_vec = rack->source;
                
                source_vec.back()->writeData(data);
                source_vec.pop_back();
                
                rack->send<MessageSwap>(source_vec);
                return true;
            }
            
            return false;
        }
        
        //==============================================================================================================
        int getSizeInUnits() override
        {
            return static_cast<int>(sizeof(*this));
        }
    
    private:
        AudioProcessorRack *rack;
        juce::String       itemId;
        juce::ValueTree    data;
    };
    
    class AudioProcessorRack::UndoableRemove final : public juce::UndoableAction
    {
    public:
        UndoableRemove(AudioProcessorRack *parRack, int parItemIndex, juce::String parItemId)
            : rack(parRack), data("Data"), itemId(std::move(parItemId)), itemIndex(parItemIndex)
        {}
        
        //==============================================================================================================
        bool perform() override
        {
            if (rack)
            {
                juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
                ProcessorVector &source_vec = rack->source;
                
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
                ProcessorVector &source_vec = rack->source;
                
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
        
        //==============================================================================================================
        int getSizeInUnits() override
        {
            return static_cast<int>(sizeof(*this));
        }
    
    private:
        AudioProcessorRack *rack;
        juce::ValueTree    data;
        juce::String       itemId;
        int                itemIndex;
    };
    
    class AudioProcessorRack::UndoableClear final : public juce::UndoableAction
    {
    public:
        explicit UndoableClear(AudioProcessorRack *parRack)
            : rack(parRack), data("Data")
        {}
        
        //==============================================================================================================
        bool perform() override
        {
            if (rack)
            {
                juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
                ProcessorVector &source_vec = rack->source;
                
                if (!source_vec.empty())
                {
                    rack->writeData(data);
                    source_vec.clear();
                    rack->send<MessageSwap>(source_vec);
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
        
        //==============================================================================================================
        int getSizeInUnits() override
        {
            return static_cast<int>(sizeof(*this));
        }
    
    private:
        AudioProcessorRack *rack;
        juce::ValueTree    data;
    };
    
    class AudioProcessorRack::UndoableMove final : public juce::UndoableAction
    {
    public:
        UndoableMove(AudioProcessorRack *parRack, int parFrom, int parTo)
            : rack(parRack),
              newIndex(parTo),
              oldIndex(parFrom)
        {}
        
        //==============================================================================================================
        bool perform() override
        {
            if (rack)
            {
                juce::CriticalSection::ScopedLockType lock(rack->sourceLock);
                
                ProcessorVector &source_vec = rack->source;
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
    
                ProcessorVector &source_vec = rack->source;
                makeRotation(newIndex, oldIndex);
                rack->send<MessageSwap>(source_vec);
                
                return true;
            }
            
            return false;
        }
        
        //==============================================================================================================
        int getSizeInUnits() override
        {
            return static_cast<int>(sizeof(*this));
        }
    
    private:
        AudioProcessorRack *rack;
        int                newIndex;
        int                oldIndex;
        
        //==============================================================================================================
        void makeRotation(int parIndexToMove, int parTargetIndex)
        {
            ProcessorVector &source_vec = rack->source;
            
            if (parIndexToMove < parTargetIndex)
            {
                for (int i = parIndexToMove; i < parTargetIndex; ++i)
                {
                    std::swap(source_vec.at(static_cast<std::size_t>(i)),
                              source_vec.at(static_cast<std::size_t>(i) + 1));
                }
            }
            else
            {
                for (int i = parIndexToMove; i > parTargetIndex; --i)
                {
                    std::swap(source_vec.at(static_cast<std::size_t>(i)),
                              source_vec.at(static_cast<std::size_t>(i) - 1));
                }
            }
        }
    };
}
//======================================================================================================================
// endregion UndoHistory
//**********************************************************************************************************************
// region Messages
//======================================================================================================================
namespace jaut
{
    class AudioProcessorRack::MessageSwap final : public IMessage
    {
    public:
        explicit MessageSwap(ProcessorVector parSource) noexcept
            : sourceCopy(std::move(parSource))
        {}
        
        //==============================================================================================================
        void handleMessage(IMessageHandler *parContext, MessageDirection) override
        {
            AudioProcessorRack &rack = *static_cast<AudioProcessorRack*>(parContext); // NOLINT
            std::swap(sourceCopy, rack.devices);
        }
    
    private:
        ProcessorVector sourceCopy;
    };
}
//======================================================================================================================
// endregion Messages
//**********************************************************************************************************************
// region AudioProcessorRack
//======================================================================================================================
namespace jaut
{
    AudioProcessorRack::AudioProcessorRack(InitCallback parInitCallback, juce::UndoManager *parUndoManager) noexcept
        : initialisationCallback(std::move(parInitCallback)),
          undoManager(parUndoManager)
    {}
    
    //==================================================================================================================
    bool AudioProcessorRack::addProcessor(const juce::String &parProcessorId)
    {
        const juce::String id = ::prepareId(parProcessorId);
        return (undoManager ? undoManager->perform(new UndoableAdd(this, id)) : UndoableAdd(this, id).perform());
    }
    
    bool AudioProcessorRack::removeProcessor(int parIndex)
    {
        if (!(parIndex >= 0 && parIndex < source.size()))
        {
            return false;
        }
        
        const juce::String id = ::prepareId(getProcessor(parIndex)->getName());
        
        return (undoManager
                    ? undoManager->perform(new UndoableRemove(this, parIndex, id))
                    : UndoableRemove(this, parIndex, id).perform());
    }
    
    bool AudioProcessorRack::moveProcessor(int parFrom, int parTo)
    {
        const int vec_end     = (getNumProcessors() - 1);
        const int adjusted_to = std::clamp<int>(parTo, 0, vec_end);
        
        if (   (vec_end < 1)
            || (parFrom < 0 || parFrom > vec_end)
            || (parFrom == adjusted_to))
        {
            return false;
        }
        
        return (undoManager
                    ? undoManager->perform(new UndoableMove(this, parFrom, adjusted_to))
                    : UndoableMove(this, parFrom, adjusted_to).perform());
    }
    
    int AudioProcessorRack::clear()
    {
        const int elements = getNumProcessors();
        
        if ((undoManager
             ? undoManager->perform(new UndoableClear(this))
             : UndoableClear(this).perform()))
        {
            return elements;
        }
        
        return 0;
    }
    
    //==================================================================================================================
    juce::String AudioProcessorRack::getName() const
    {
        return "jaut:AudioProcessorRack";
    }
    
    //==================================================================================================================
    int AudioProcessorRack::getNumProcessors() const noexcept
    {
        return static_cast<int>(source.size());
    }
    
    //==================================================================================================================
    AudioProcessorRack::Processor* AudioProcessorRack::getProcessor(int parIndex) noexcept
    {
        return getProcessor_impl(*this, parIndex);
    }
    
    const AudioProcessorRack::Processor* AudioProcessorRack::getProcessor(int parIndex) const noexcept
    {
        return getProcessor_impl(*this, parIndex);
    }
    
    //==================================================================================================================
    void AudioProcessorRack::process(juce::AudioBuffer<float> &parBuffer, juce::MidiBuffer &parMidiBuffer)
    {
        processNextMessage();
        
        for (auto &device : devices)
        {
            device->process(parBuffer, parMidiBuffer);
        }
    }
    
    void AudioProcessorRack::process(juce::AudioBuffer<double> &parBuffer, juce::MidiBuffer &parMidiBuffer)
    {
        processNextMessage();
    
        for (auto &device : devices)
        {
            device->process(parBuffer, parMidiBuffer);
        }
    }
    
    void AudioProcessorRack::prepare(ProcessSpec parSpec)
    {
        for (auto &device : devices)
        {
            device->prepare(parSpec);
        }
        
        std::swap(processSpec, parSpec);
    }
    
    void AudioProcessorRack::release()
    {
        for (auto &device : devices)
        {
            device->release();
        }
    }
    
    void AudioProcessorRack::reset()
    {
        for (auto &device : devices)
        {
            device->reset();
        }
    }
    
    //==================================================================================================================
    void AudioProcessorRack::readData(const juce::ValueTree parData)
    {
        ProcessorVector data_vector;
        data_vector.reserve(static_cast<std::size_t>(parData.getNumChildren()));
        
        for (const auto &child : parData)
        {
            if (child.hasType("RackDevice"))
            {
                const juce::String id = child.getProperty("Id", "");
    
                if (auto device = initialisationCallback(id))
                {
                    const juce::ValueTree device_data = child.getChild(0);
                    
                    if (device_data.isValid())
                    {
                        device->readData(device_data);
                    }
                    
                    data_vector.emplace_back(std::move(device));
                }
            }
        }
        
        data_vector.shrink_to_fit();
        
        {
            const juce::CriticalSection::ScopedLockType lock(sourceLock);
    
            std::swap(source, data_vector);
            send<MessageSwap>(source);
        }
    }
    
    void AudioProcessorRack::writeData(juce::ValueTree parData)
    {
        parData.removeAllChildren(nullptr);
        
        {
            const juce::CriticalSection::ScopedLockType lock(sourceLock);
        
            for (auto &device : source)
            {
                const juce::String id = ::prepareId(device->getName());
                
                juce::ValueTree device_state("RackDevice");
                (void) device_state.setProperty("Id", id, nullptr);
                
                juce::ValueTree device_data("Data");
                device->writeData(device_data);
                device_state.appendChild(device_data, nullptr);
    
                parData.appendChild(device_state, nullptr);
            }
        }
    }
}
//======================================================================================================================
// endregion AudioProcessorRack
//**********************************************************************************************************************
