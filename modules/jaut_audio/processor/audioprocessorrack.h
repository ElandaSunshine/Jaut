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
class JAUT_API AudioProcessorRack final : public SerializableAudioProcessor, public MessageHandler<16>
{
public:
    enum JAUT_API ProcessingMode
    {
        Serial,
        Single
    };

    //==================================================================================================================
    using Processor        = SerializableAudioProcessor*;
    using ProcessorPointer = std::shared_ptr<std::remove_pointer_t<Processor>>;
    using ProcessorVector  = std::vector<ProcessorPointer>;
    using InitializerFunc  = std::function<ProcessorPointer(const String&)>;
    using LockType         = SpinLock::ScopedLockType;

    //==================================================================================================================
    AudioProcessorRack(AudioProcessor &processor, ProcessingMode mode, AudioProcessorValueTreeState &vts,
                       InitializerFunc processorInitializer, UndoManager *undoManager = nullptr) noexcept;

    //==================================================================================================================
    bool addDevice(const String &deviceId);
    bool removeDevice(const String &deviceId);
    bool moveDevice(const String &deviceId, int index);
    bool clear();

    //==================================================================================================================
    const String getName() const override;
    bool hasEditor() const override { return false; }

    //==================================================================================================================
    int getNumDevices() const noexcept;

    //==================================================================================================================
    Processor getDevice(int index) noexcept;
    Processor getDevice(const String &deviceId) noexcept;
    Processor getDevice(int index) const noexcept;
    Processor getDevice(const String &deviceId) const noexcept;

    //==================================================================================================================
    Processor getActivated();
    String getActivatedId() const;
    int getActiveIndex() const;
    bool setActivated(int index);
    bool setActivated(const String &id);

    //==================================================================================================================
    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer) override;
    void processBlock(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer) override;
    void prepareToPlay(double sampleRate, int maxBlockSamples) override;
    void releaseResources() override;

    //==================================================================================================================
    void readData(const ValueTree data) override;
    void writeData(ValueTree data) override;

private:
    template<bool> class UndoableAddRemove;
    class UndoableClear;
    class UndoableMove;
    class MessageSwap;

    //==================================================================================================================
    AudioProcessor &parent;
    AudioProcessorValueTreeState &parameters;
    UndoManager *undoManager;

    SpinLock swapLock;
    ProcessorVector devices, source;
    InitializerFunc initializerFunction;
    ProcessingMode mode;
    std::atomic<int> activeIndex;
    int previousIndex;

    //==================================================================================================================
    void sendSwapMessage();
};
}
