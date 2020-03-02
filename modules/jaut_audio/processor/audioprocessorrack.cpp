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

#include <jaut/processor/audioprocessorrack.h>
#include <jaut/jaut_util/exception.h>

//region Namespace
/* ================================================================================================================== */
namespace
{
using namespace jaut;

inline int hasProcessor(const AudioProcessorRack::ProcessorVector &rack, const String &id)
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
template<>
class AudioProcessorRack::UndoableAddRemove<true> final : public UndoableAction
{
public:
    UndoableAddRemove(AudioProcessorRack *rack, String itemId)
        : rack(rack), itemId(std::move(itemId))
    {}

    //==================================================================================================================
    bool perform() override
    {
        if(!rack)
        {
            const String id = itemId.removeCharacters(" ").toLowerCase();

            if(::hasProcessor(rack->source, id) == -1)
            {
                if(auto new_processor = rack->initializerFunction(id))
                {
                    if(data.isValid())
                    {
                        new_processor->readData(data);
                    }

                    new_processor->prepareToPlay(rack->getSampleRate(), rack->getBlockSize());
                    new_processor->setPlayHead(rack->getPlayHead());

                    {
                        SpinLock::ScopedLockType locker(rack->swapLock);
                        rack->source.emplace_back(new_processor);
                    }

                    rack->sendSwapMessage();
                    return true;
                }
            }
        }

        return false;
    }

    bool undo() override
    {
        if(!rack)
        {
            const String id           = itemId.removeCharacters(" ").toLowerCase();
            const int processor_index = ::hasProcessor(rack->source, id);

            if(processor_index >= 0)
            {
                data = ValueTree("Data");
                rack->source.at(processor_index)->writeData(data);

                {
                    SpinLock::ScopedLockType locker(rack->swapLock);
                    rack->source.erase(rack->source.begin() + processor_index);
                }

                if(rack->getActiveIndex() >= rack->source.size())
                {
                    rack->activeIndex.store(rack->source.size() - 1);
                }

                rack->sendSwapMessage();
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
};

template<>
class AudioProcessorRack::UndoableAddRemove<false> final : public UndoableAction
{
public:
    UndoableAddRemove(AudioProcessorRack *rack, String itemId)
            : rack(rack), itemId(std::move(itemId)), data("Data")
    {}

    //==================================================================================================================
    bool perform() override
    {
        if(!rack)
        {
            const String id           = itemId.removeCharacters(" ").toLowerCase();
            const int processor_index = ::hasProcessor(rack->source, id);

            if(processor_index >= 0)
            {
                rack->source.at(processor_index)->writeData(data);

                {
                    SpinLock::ScopedLockType locker(rack->swapLock);
                    rack->source.erase(rack->source.begin() + processor_index);
                }

                if(rack->getActiveIndex() >= rack->source.size())
                {
                    rack->activeIndex.store(rack->source.size() - 1);
                }

                rack->sendSwapMessage();
                return true;
            }
        }

        return false;
    }

    bool undo() override
    {
        if(!rack)
        {
            const String id = itemId.removeCharacters(" ").toLowerCase();

            if(::hasProcessor(rack->source, id) == -1)
            {
                if(auto new_processor = rack->initializerFunction(id))
                {
                    new_processor->readData(data);
                    new_processor->prepareToPlay(rack->getSampleRate(), rack->getBlockSize());
                    new_processor->setPlayHead(rack->getPlayHead());

                    {
                        SpinLock::ScopedLockType locker(rack->swapLock);
                        rack->source.emplace_back(new_processor);
                    }

                    rack->sendSwapMessage();
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
};

class AudioProcessorRack::UndoableClear final : public UndoableAction
{
public:
    explicit UndoableClear(AudioProcessorRack *rack)
        : rack(rack), data("Data")
    {}

    //==================================================================================================================
    bool perform() override
    {
        if(!rack && !rack->source.empty())
        {
            rack->writeData(data);

            {
                SpinLock::ScopedLockType locker(rack->swapLock);
                rack->source.clear();
            }

            rack->activeIndex.store(0);
            rack->sendSwapMessage();

            return true;
        }

        return false;
    }

    bool undo() override
    {
        if(!rack && data.isValid())
        {
            {
                SpinLock::ScopedLockType locker(rack->swapLock);
                rack->readData(data);
            }

            rack->sendSwapMessage();
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
        if(!rack)
        {
            const String id           = itemId.removeCharacters(" ").toLowerCase();
            const int processor_index = ::hasProcessor(rack->source, id);

            if(processor_index >= 0 && processor_index != itemIndex &&
               jaut::fit_s<int>(itemIndex, 0, rack->source.size()))
            {
                {
                    SpinLock::ScopedLockType locker(rack->swapLock);
                    makeRotation(processor_index, itemIndex);
                }

                itemIndex = processor_index;
                rack->sendSwapMessage();

                return true;
            }
        }

        return false;
    }

    bool undo() override
    {
        if(!rack)
        {
            const String id           = itemId.removeCharacters(" ").toLowerCase();
            const int processor_index = ::hasProcessor(rack->source, id);

            {
                SpinLock::ScopedLockType locker(rack->swapLock);
                makeRotation(processor_index, itemIndex);
            }

            itemIndex = processor_index;
            rack->sendSwapMessage();

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
//endregion UndoHistory
//region Messages
/* ================================================================================================================== */
class AudioProcessorRack::MessageSwap final : public IMessage
{
public:
    explicit MessageSwap(ProcessorVector source)
        : sourceCopy(std::move(source))
    {}

    //==================================================================================================================
    void handleMessage(IMessageHandler *context, MessageDirection messageDirection) override
    {
        AudioProcessorRack &rack = *static_cast<AudioProcessorRack*>(context);

        if(messageDirection == TargetThread)
        {
            if(rack.swapLock.tryEnter())
            {
                sourceCopy = rack.source;
                rack.swapLock.exit();
            }

            std::swap(sourceCopy, rack.devices);
        }
        else
        {
            std::swap(rack.source, sourceCopy);
        }
    }

private:
    ProcessorVector sourceCopy;
};
//endregion Messages
//region AudioProcessorRack
/* ==================================================================================
 * =============================== AudioProcessorRack ===============================
 * ================================================================================== */
AudioProcessorRack::AudioProcessorRack(AudioProcessor &processor, ProcessingMode mode,
                                       AudioProcessorValueTreeState &vts, InitializerFunc processorInitializer,
                                       UndoManager *undoManager) noexcept
    : parent(processor), parameters(vts), undoManager(undoManager), initializerFunction(processorInitializer),
      mode(mode), activeIndex(0), previousIndex(0)
{
    JAUT_ENSURE_MESSAGE_THREAD();
}

//======================================================================================================================
bool AudioProcessorRack::addDevice(const String &deviceId)
{
    JAUT_ENSURE_MESSAGE_THREAD();
    return undoManager ? undoManager->perform(new UndoableAddRemove<true>(this, deviceId)) :
                         UndoableAddRemove<true>(this, deviceId).perform();
}

bool AudioProcessorRack::removeDevice(const String &deviceId)
{
    JAUT_ENSURE_MESSAGE_THREAD();
    return undoManager ? undoManager->perform(new UndoableAddRemove<false>(this, deviceId)) :
                         UndoableAddRemove<false>(this, deviceId).perform();
}

bool AudioProcessorRack::moveDevice(const String &deviceId, int index)
{
    JAUT_ENSURE_MESSAGE_THREAD();
    return undoManager ? undoManager->perform(new UndoableMove(this, deviceId, index)) :
                         UndoableMove(this, deviceId, index).perform();
}

bool AudioProcessorRack::clear()
{
    JAUT_ENSURE_MESSAGE_THREAD();
    return undoManager ? undoManager->perform(new UndoableClear(this)) : UndoableClear(this).perform();
}

//======================================================================================================================
const String AudioProcessorRack::getName() const
{
    return "AudioProcessorRack";
}

int AudioProcessorRack::getNumDevices() const noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD();
    return source.size();
}

//==================================================================================================================
AudioProcessorRack::Processor AudioProcessorRack::getDevice(int index) noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD();
    return jaut::fit_s(index, 0, getNumDevices()) ? source.at(index).get() : nullptr;
}

AudioProcessorRack::Processor AudioProcessorRack::getDevice(const String &deviceId) noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD();

    const String id = deviceId.removeCharacters(" ").toLowerCase();
    int index       = ::hasProcessor(source, id);

    return index >= 0 ? source.at(index).get() : nullptr;
}

AudioProcessorRack::Processor AudioProcessorRack::getDevice(int index) const noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD();
    return jaut::fit_s(index, 0, getNumDevices()) ? source.at(index).get() : nullptr;
}

AudioProcessorRack::Processor AudioProcessorRack::getDevice(const String &deviceId) const noexcept
{
    JAUT_ENSURE_MESSAGE_THREAD();

    const String id = deviceId.removeCharacters(" ").toLowerCase();
    int index       = ::hasProcessor(source, id);

    return index >= 0 ? source.at(index).get() : nullptr;
}

//======================================================================================================================
AudioProcessorRack::Processor AudioProcessorRack::getActivated()
{
    if(mode == Serial)
    {
        return nullptr;
    }

    return source.at(getActiveIndex()).get();
}

String AudioProcessorRack::getActivatedId() const
{
    if(mode == Serial)
    {
        return String();
    }

    return source.at(getActiveIndex())->getName().removeCharacters(" ").toLowerCase();
}

int AudioProcessorRack::getActiveIndex() const
{
    return activeIndex.load();
}

bool AudioProcessorRack::setActivated(int index)
{
    if(mode == Serial)
    {
        return false;
    }

    const int active_index = getActiveIndex();

    if(index != active_index && index >= 0 && index < source.size())
    {
        previousIndex = active_index;
        activeIndex.store(index);

        return true;
    }

    return false;
}

bool AudioProcessorRack::setActivated(const String &id)
{
    if(mode == Serial)
    {
        return false;
    }

    const int index        = ::hasProcessor(source, id.removeCharacters(" ").toLowerCase());
    const int active_index = getActiveIndex();

    if(index != active_index && index >= 0)
    {
        previousIndex = active_index;
        activeIndex.store(index);

        return true;
    }

    return false;
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

        int devices_size = devices.size();
        int index        = activeIndex.load();

        if(index >= devices_size)
        {
            index = previousIndex < devices_size ? previousIndex : devices_size - 1;
        }

        devices.at(index)->processBlock(buffer, midiBuffer);
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

        int devices_size = devices.size();
        int index        = activeIndex.load();

        if(index >= devices_size)
        {
            index = previousIndex < devices_size ? previousIndex : devices_size - 1;
        }

        devices.at(index)->processBlock(buffer, midiBuffer);
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
    data_vector.reserve(data.getNumChildren());

    for(const auto &child : data)
    {
        if(child.hasType("RackDevice"))
        {
            const String id = child.getProperty("Id", "");

            if(auto device = initializerFunction(id))
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

    if(MessageManager::getInstance()->isThisTheMessageThread())
    {
        std::swap(source, data_vector);
        sendSwapMessage();
    }
    else
    {
        {
            SpinLock::ScopedLockType locker(swapLock);
            source = data_vector;
        }

        std::swap(devices, data_vector);
    }

    activeIndex.store(data.getProperty("ActiveIndex", 0));
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

//======================================================================================================================
void AudioProcessorRack::sendSwapMessage()
{
    cancelPendingMessages();
    send<MessageSwap>(source);
}
//endregion AudioProcessorRack
}
