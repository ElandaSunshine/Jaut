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

#include <jaut/expo.h>
#include <jaut/dspunit.h>

namespace jaut
{
class JAUT_API AudioProcessorRack final : public jaut::DspUnit
{
public:
    using int_Connection = int;
    using t_ConnectPin   = std::atomic<int_Connection>;
    using t_CallbackLock = CriticalSection::ScopedLockType;

    class JAUT_API RackDevice final
    {
    public:
        //==============================================================================================================
        RackDevice(DspUnit &unit);
        RackDevice(DspUnit *unit, bool isLinked);
        RackDevice(RackDevice &&other);
        ~RackDevice();

        //==============================================================================================================
        RackDevice &operator=(RackDevice &&right);

        //==============================================================================================================
        bool operator >(const RackDevice &right) const noexcept;
        bool operator <(const RackDevice &right) const noexcept;
        bool operator==(const RackDevice &right) const noexcept;
        bool operator!=(const RackDevice &right) const noexcept;
        bool operator>=(const RackDevice &right) const noexcept;
        bool operator<=(const RackDevice &right) const noexcept;

        //==============================================================================================================
        DspUnit *operator->() const;
        DspUnit &operator*() const;

        //==============================================================================================================
        String getId() const noexcept;
        bool isLinkedUnit() const noexcept;
        DspUnit *getUnit() const noexcept;
        AudioProcessorRack *getRack() const noexcept;
        int_Connection getConnectionIndex() const noexcept;

        //==============================================================================================================
        RackDevice *getPreviousDevice() const noexcept;
        RackDevice *getNextDevice() const noexcept;

        //==============================================================================================================
        void connectWith(int_Connection index);

        //==============================================================================================================
        friend void swap(RackDevice &left, RackDevice &right)
        {
            std::swap(left.rack, right.rack);
            std::swap(left.unit, right.unit);
            std::swap(left.linked, right.linked);
            left.connection.store(right.connection.exchange(left.connection));
            left.id.swapWith(right.id);
        }

    private:
        friend class AudioProcessorRack;
        friend class Iterator;

        AudioProcessorRack *rack;
        t_ConnectPin connection;
        DspUnit *unit;
        bool linked;
        String id;

        //==============================================================================================================
        void setRack(AudioProcessorRack*);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RackDevice)
    };

    //==================================================================================================================
    using t_Rack          = std::vector<RackDevice>;
    using t_Iterator      = t_Rack::iterator;
    using t_ConstIterator = t_Rack::const_iterator;

    //==================================================================================================================
    AudioProcessorRack(AudioProcessor &processor, AudioProcessorValueTreeState *vts = nullptr,
                       UndoManager *undoManager = nullptr);

    //==================================================================================================================
    void addDevice(DspUnit &unit);
    void addDevice(DspUnit *unit, bool isLinked = false);
    void removeDevice(int index);
    void removeDevice(const String &id);
    void removeDevice(const DspUnit &unit);
    void removeDevice(const RackDevice &device);

    //==================================================================================================================
    const String getName() const override;
    bool hasEditor() const override { return false; }
    void clear();
    int getNumDevices() const noexcept;

    //==================================================================================================================
    RackDevice *getDevice(int index) noexcept;
    RackDevice *getDevice(const String &id) noexcept;
    const RackDevice *getDevice(int index) const noexcept;
    const RackDevice *getDevice(const String &id) const noexcept;

    //==================================================================================================================
    void connectDevice(int_Connection deviceIndex);
    int_Connection getConnectionIndex() const noexcept;

    //==================================================================================================================
    virtual void process(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer) override;
    virtual void process(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer) override;
    virtual void beginPlayback(double sampleRate, int maxBlockSamples) override;
    virtual void finishPlayback() override;

    //==================================================================================================================
    void createLinearChain();
    void reverseChain();
    void createSoloChain(int index);

    //==================================================================================================================
    virtual void readData(const ValueTree data) override;
    virtual void writeData(ValueTree data) const override;

    //==================================================================================================================
    t_Iterator begin() noexcept;
    t_Iterator end()   noexcept;
    t_ConstIterator begin() const noexcept;
    t_ConstIterator end()   const noexcept;

private:
    t_ConnectPin connection;
    std::vector<RackDevice> devices;
    bool closedChain;

    //==================================================================================================================
    jaut::DspGui *getGuiType() override { return nullptr; }
    void traverseChain(std::function<void(RackDevice&,bool&)> callback);
    RackDevice &setAndReturnPrevious(int previousIndex, int currentIndex);
};
}
