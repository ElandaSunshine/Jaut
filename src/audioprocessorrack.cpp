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

#include <jaut/audioprocessorrack.h>
#include <jaut/exception.h>



#pragma region Namespace
namespace
{
using namespace jaut;

inline const AudioProcessorRack::RackDevice *findDevice(const std::vector<AudioProcessorRack::RackDevice> &devices,
                                                        const String &id) noexcept
{
    for(auto &device : devices)
    {
        if(device.getId() == id)
        {
            return &device;
        }
    }

    return nullptr;
}

inline AudioProcessorRack::RackDevice *findDevice(std::vector<AudioProcessorRack::RackDevice> &devices,
                                                  const String &id) noexcept
{
    for(auto &device : devices)
    {
        if(device.getId() == id)
        {
            return &device;
        }
    }

    return nullptr;
}

inline void checkAndRemoveDevice(std::vector<AudioProcessorRack::RackDevice> &devices)
{
    for(auto &device : devices)
    {
        if(!jaut::fit_s<int>(device.getConnectionIndex(), -2, devices.size()))
        {
            device.connectWith(-1);
        }
    }
}

inline bool doesProcessorChainOutput(int startIndex, std::vector<AudioProcessorRack::RackDevice> &devices)
{
    for(int i = 0; i < devices.size(); ++i)
    {
        if(startIndex < 0)
        {
            break;
        }
        else if(startIndex >= devices.size())
        {
            startIndex = -1;
            break;
        }
        else
        {
            startIndex = devices.at(startIndex).getConnectionIndex();
        }
    }

    return startIndex == -2;
}
}
#pragma endregion Namespace



namespace jaut
{
#pragma region AudioProcessorRack::RackDevice
/* ==================================================================================
 * =================================== RackDevice ===================================
 * ================================================================================== */
AudioProcessorRack::RackDevice::RackDevice(DspUnit &unit)
    : connection(-2), unit(&unit), linked(true),
      id(unit.getName().removeCharacters(" ").toLowerCase())
{}

AudioProcessorRack::RackDevice::RackDevice(DspUnit *unit, bool isLinked)
    : connection(-2), unit(unit), linked(isLinked),
      id(unit ? unit->getName().removeCharacters(" ").toLowerCase() : "")
{}

AudioProcessorRack::RackDevice::RackDevice(RackDevice &&other)
    : rack(std::move(other.rack)),
      connection(std::move(other.connection)),
      unit(std::move(other.unit)),
      linked(std::move(other.linked)),
      id(std::move(other.id))
{
    other.rack = nullptr;
    other.unit = nullptr;
}

AudioProcessorRack::RackDevice::~RackDevice()
{
    if(!linked)
    {
        delete unit;
    }
    else
    {
        unit->setPlayHead(nullptr);
        unit->reset();
    }
}

//======================================================================================================================
AudioProcessorRack::RackDevice &AudioProcessorRack::RackDevice::operator=(RackDevice &&right)
{
    swap(*this, right);
    right.rack = nullptr;
    right.unit = nullptr;

    return *this;
}

//======================================================================================================================
bool AudioProcessorRack::RackDevice::operator>(const RackDevice &right) const noexcept
{
    bool found_this = false;
    bool found_next = false;
    
    if(rack && right != *this)
    {
        rack->traverseChain([this, &right, &found_this, &found_next](RackDevice &device, bool &cancel)
        {
            if(found_next)
            {
                cancel = true;
                return;
            }

            if(device == *this)
            {
                found_this = true;
            }
            else if(device == right)
            {
                found_next = true;
            }
        });
    }

    return found_this && found_next;
}

bool AudioProcessorRack::RackDevice::operator<(const RackDevice &right) const noexcept
{
    bool found_prev = false;
    bool found_this = false;
    
    if(rack && right != *this)
    {
        rack->traverseChain([this, &right, &found_this, &found_prev](RackDevice &device, bool &cancel)
        {
            if(found_this)
            {
                cancel = true;
                return;
            }

            if(device == right)
            {
                found_prev = true;
            }
            else if(device == *this)
            {
                found_this = true;
            }
        });
    }

    return found_prev && found_this;
}

bool AudioProcessorRack::RackDevice::operator==(const RackDevice &right) const noexcept
{
    return unit == right.unit;
}

bool AudioProcessorRack::RackDevice::operator!=(const RackDevice &right) const noexcept
{
    return unit != right.unit;
}

bool AudioProcessorRack::RackDevice::operator>=(const RackDevice &right) const noexcept
{
    if(right != *this)
    {
        return *this > right;
    }

    return true;
}

bool AudioProcessorRack::RackDevice::operator<=(const RackDevice &right) const noexcept
{
    if(right != *this)
    {
        return *this < right;
    }

    return true;
}

//======================================================================================================================
DspUnit *AudioProcessorRack::RackDevice::operator->() const
{
    if(unit)
    {
        return unit;
    }

    throw exception::NullDereference("RackDevice '" + getId() + "' (->)");
}

DspUnit &AudioProcessorRack::RackDevice::operator*() const
{
    if(unit)
    {
        return *unit;
    }

    throw exception::NullDereference("RackDevice '" + getId() + "' (*)");
}

//======================================================================================================================
String AudioProcessorRack::RackDevice::getId() const noexcept
{
    return id;
}

bool AudioProcessorRack::RackDevice::isLinkedUnit() const noexcept
{
    return linked;
}

DspUnit *AudioProcessorRack::RackDevice::getUnit() const noexcept
{
    return unit;
}

AudioProcessorRack *AudioProcessorRack::RackDevice::getRack() const noexcept
{
    return rack;
}

AudioProcessorRack::int_Connection AudioProcessorRack::RackDevice::getConnectionIndex() const noexcept
{
    return connection;
}

//======================================================================================================================
AudioProcessorRack::RackDevice *AudioProcessorRack::RackDevice::getPreviousDevice() const noexcept
{
    RackDevice *result = nullptr;

    if(rack)
    {
        rack->traverseChain([this, &result](RackDevice &device, bool &cancel)
        {
            if(jaut::fit_s(device.connection, 0, rack->getNumDevices()))
            {
                RackDevice &next_device = rack->devices.at(device.connection);

                if(next_device == *this)
                {
                    result = &device;
                    cancel = true;
                }
            }
        });
    }

    return result;
}

AudioProcessorRack::RackDevice *AudioProcessorRack::RackDevice::getNextDevice() const noexcept
{
    RackDevice *result = nullptr;

    if(rack)
    {
        rack->traverseChain([this, &result](RackDevice &device, bool &cancel)
        {
            if(device == *this && jaut::fit_s(this->connection, 0, rack->getNumDevices()))
            {
                result = &rack->devices.at(this->connection);
                cancel = true;
            }
        });
    }

    return result;
}

//======================================================================================================================
void AudioProcessorRack::RackDevice::connectWith(int_Connection index)
{
    JAUT_ENSURE_AUDIO_THREAD();

    if(!rack)
    {
        return;
    }

    if(!jaut::fit_s(index, -2, rack->getNumDevices()))
    {
        jassertfalse; // Index out of bounds, device must either be inside size of added devices
                      // or -1 for no device, or -2 for output
        return;
    }

    connection = index;
}

//======================================================================================================================
void AudioProcessorRack::RackDevice::setRack(AudioProcessorRack *rack)
{
    this->rack = rack;

    if(rack)
    {
        unit->setPlayHead(rack->getPlayHead());
    }
}
#pragma endregion AudioProcessorRack::RackDevice



#pragma region AudioProcessorRack
/* ==================================================================================
 * =============================== AudioProcessorRack ===============================
 * ================================================================================== */
AudioProcessorRack::AudioProcessorRack(AudioProcessor &processor, AudioProcessorValueTreeState *vts,
                                       UndoManager *undoManager)
    : DspUnit(processor, vts, undoManager),
      connection(0), closedChain(false)
{}

//======================================================================================================================
void AudioProcessorRack::addDevice(DspUnit &unit)
{
    RackDevice device(unit);

    if(!::findDevice(devices, device.getId()))
    {
        RackDevice *const last_device = getNumDevices() > 0 ? &devices.at(getNumDevices() - 1) : nullptr;

        device.setRack(this);
        (void) devices.emplace_back(std::move(device));

        if(last_device && last_device->connection == -2)
        {
            last_device->connection = getNumDevices() - 1;
        }
    }
}

void AudioProcessorRack::addDevice(DspUnit *unit, bool isLinked)
{
    RackDevice device(unit, isLinked);

    if(!::findDevice(devices, device.getId()))
    {
        RackDevice *const last_device = getNumDevices() > 0 ? &devices.at(getNumDevices() - 1) : nullptr;
        
        device.setRack(this);
        (void) devices.emplace_back(std::move(device));

        if(last_device && last_device->connection == -2)
        {
            last_device->connection = getNumDevices() - 1;
        }
    }
}

void AudioProcessorRack::removeDevice(int index)
{
    if(jaut::fit_s<int>(index, 0, devices.size()))
    {
        (void) devices.erase(devices.begin() + index);
        ::checkAndRemoveDevice(devices);
        closedChain = ::doesProcessorChainOutput(connection, devices);
    }
}

void AudioProcessorRack::removeDevice(const String &id)
{
    for(int i = 0; i < devices.size(); ++i)
    {
        if(devices.at(i).getId().equalsIgnoreCase(id.removeCharacters(" ")))
        {
            (void) devices.erase(devices.begin() + i);
            ::checkAndRemoveDevice(devices);
            closedChain = ::doesProcessorChainOutput(connection, devices);

            return;
        }
    }
}

void AudioProcessorRack::removeDevice(const DspUnit &unit)
{
    for(int i = 0; i < devices.size(); ++i)
    {
        if(&unit == devices.at(i).getUnit())
        {
            (void) devices.erase(devices.begin() + i);
            ::checkAndRemoveDevice(devices);
            closedChain = ::doesProcessorChainOutput(connection, devices);

            return;
        }
    }
}

void AudioProcessorRack::removeDevice(const RackDevice &device)
{
    for(int i = 0; i < devices.size(); ++i)
    {
        if(device == devices.at(i))
        {
            (void) devices.erase(devices.begin() + i);
            ::checkAndRemoveDevice(devices);
            closedChain = ::doesProcessorChainOutput(connection, devices);

            return;
        }
    }
}

//======================================================================================================================
const String AudioProcessorRack::getName() const
{
    return "Rack Processor";
}

void AudioProcessorRack::clear()
{
    devices.clear();
    connection == 0;
    closedChain = false;
}

int AudioProcessorRack::getNumDevices() const noexcept
{
    return devices.size();
}

//==================================================================================================================
AudioProcessorRack::RackDevice *AudioProcessorRack::getDevice(int index) noexcept
{
    return jaut::fit_s(index, 0, getNumDevices()) ? &devices.at(index) : nullptr;
}

AudioProcessorRack::RackDevice *AudioProcessorRack::getDevice(const String &id) noexcept
{
    return ::findDevice(devices, id);
}

const AudioProcessorRack::RackDevice *AudioProcessorRack::getDevice(int index) const noexcept
{
    return jaut::fit_s(index, 0, getNumDevices()) ? &devices.at(index) : nullptr;
}

const AudioProcessorRack::RackDevice *AudioProcessorRack::getDevice(const String &id) const noexcept
{
    return ::findDevice(devices, id);
}

//==================================================================================================================
void AudioProcessorRack::connectDevice(int_Connection deviceIndex)
{
    if(!jaut::fit_s(deviceIndex, -1, getNumDevices()))
    {
        jassertfalse; // Index out of bounds, device must either be inside size of added devices
                      // or -1 denoting no device
        return;
    }

    connection  = deviceIndex;
    closedChain = ::doesProcessorChainOutput(connection, devices);
}

AudioProcessorRack::int_Connection AudioProcessorRack::getConnectionIndex() const noexcept
{
    return connection;
}

//======================================================================================================================
void AudioProcessorRack::process(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer)
{
    t_CallbackLock lock (getCallbackLock());

    traverseChain([&buffer, &midiBuffer](RackDevice &device, bool&)
    {
        device->processBlock(buffer, midiBuffer);
    });
}

void AudioProcessorRack::process(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer)
{
    t_CallbackLock lock (getCallbackLock());

    traverseChain([&buffer, &midiBuffer](RackDevice &device, bool&)
    {
        device->processBlock(buffer, midiBuffer);
    });
}

void AudioProcessorRack::beginPlayback(double sampleRate, int maxBlockSamples)
{
    t_CallbackLock lock (getCallbackLock());

    traverseChain([&sampleRate, maxBlockSamples](RackDevice &device, bool&)
    {
        device->beginPlayback(sampleRate, maxBlockSamples);
    });
}

void AudioProcessorRack::finishPlayback()
{
    t_CallbackLock lock (getCallbackLock());

    traverseChain([](RackDevice &device, bool&)
    {
        device->finishPlayback();
    });
}

//======================================================================================================================
void AudioProcessorRack::createLinearChain()
{
    connection = 0;

    for(int i = 0; i < devices.size(); ++i)
    {
        RackDevice &device = devices.at(i);

        if(i == devices.size() - 1)
        {
            device.connection = -2;
        }
        else
        {
            device.connection = i + 1;
        }
    }

    closedChain = devices.size() > 0;
}

void AudioProcessorRack::createSoloChain(int index)
{
    if(jaut::fit_s(index, 0, getNumDevices()))
    {
        connection = index;
        devices.at(index).connection = -2;
        closedChain = true;
    }
}

//======================================================================================================================
void AudioProcessorRack::readData(const ValueTree data)
{
    if(data.isValid() && data.getNumChildren() > 0)
    {
        for(auto child : data)
        {
            if(child.hasType("Device"))
            {
                const var index_var = child.getProperty("Index", -1);
                const int index     = index_var;

                if(jaut::fit_s<int>(index, 0, devices.size()))
                {
                    RackDevice &device = devices.at(index);
                    device->readData(child);
                    device.connectWith(child.getProperty("Connection", -1));
                }
            }
        }
    }
}

void AudioProcessorRack::writeData(ValueTree data) const
{
    if(data.isValid())
    {
        data.removeAllChildren(nullptr);

        for(int i = 0; i < devices.size(); ++i)
        {
            const RackDevice &device = devices.at(i);
            ValueTree device_state("Device");

            device->writeData(device_state);
            (void) device_state.setProperty("Index", i, nullptr);
            (void) device_state.setProperty("Connection", device.getConnectionIndex(), nullptr);
            (void) device_state.setProperty("Name", device->getName(), nullptr);

            data.appendChild(device_state, nullptr);
        }
    }
}

//======================================================================================================================
AudioProcessorRack::t_Iterator AudioProcessorRack::begin() noexcept
{
    return devices.begin();
}

AudioProcessorRack::t_Iterator AudioProcessorRack::end() noexcept
{
    return devices.end();
}

AudioProcessorRack::t_ConstIterator AudioProcessorRack::begin() const noexcept
{
    return devices.begin();
}

AudioProcessorRack::t_ConstIterator AudioProcessorRack::end() const noexcept
{
    return devices.end();
}

//======================================================================================================================
void AudioProcessorRack::traverseChain(std::function<void(RackDevice&,bool&)> callback)
{
    if(!closedChain || !callback)
    {
        return;
    }

    int last_connection = connection;
    int loop_count      = 0;
    bool should_cancel  = false;

    while(last_connection != -2 && loop_count++ < getNumDevices())
    {
        RackDevice &device = devices.at(last_connection);
        callback(device, should_cancel);

        if(should_cancel)
        {
            return;
        }

        last_connection = device.connection;
    }
}
#pragma endregion AudioProcessorRack
}
