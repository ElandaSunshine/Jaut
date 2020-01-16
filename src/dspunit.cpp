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
    @file   dspunit.cpp
    @date   11, July 2019
    
    ===============================================================
 */

#include <jaut/dspunit.h>

namespace jaut
{
DspUnit::DspUnit(DspUnit &processor, AudioProcessorValueTreeState *vts, UndoManager *undoManager) noexcept
    : processor(processor),
      parameters(vts),
      history(undoManager),
      activeGui(nullptr)
{
    initialize();
}

DspUnit::DspUnit(AudioProcessor &processor, AudioProcessorValueTreeState *vts, UndoManager *undoManager) noexcept
    : processor(processor),
      parameters(vts),
      history(undoManager),
      activeGui(nullptr)
{
    initialize();
}

DspUnit::~DspUnit()
{
    this->setPlayHead(nullptr);
    this->activeGui  = nullptr;
    this->parameters = nullptr;
    this->history    = nullptr;
}

//=====================================================================================================================
void DspUnit::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiBuffer)
{
    JAUT_ENSURE_AUDIO_THREAD();

    if constexpr(internal::STRICT_PROCESSING_ENABLED())
    {
        /**
            You should not use this method when double precision processing is enabled as
            you may lose data when passing the buffer.

            If you don't care about audio data loss, turn off STRICT_PROCESSING in the expo.h file!
         */
        jassert(!isUsingDoublePrecision());
    }

    process(buffer, midiBuffer);
}

void DspUnit::processBlock(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer)
{
    JAUT_ENSURE_AUDIO_THREAD();

    if constexpr(internal::STRICT_PROCESSING_ENABLED())
    {
        /**
            You should not use this method when double precision processing is disabled as
            you may use an unnecessary amount of memory the buffer doesn't even need!

            If you don't care about memory garbage, turn off STRICT_PROCESSING in the expo.h file!
         */
        jassert(isUsingDoublePrecision());
    }

    process(buffer, midiBuffer);
}

void DspUnit::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    this->setRateAndBufferSizeDetails(sampleRate, maximumExpectedSamplesPerBlock);
    beginPlayback(sampleRate, maximumExpectedSamplesPerBlock);
}

void DspUnit::releaseResources()
{
    this->finishPlayback();
}

//=====================================================================================================================
RangedAudioParameter *DspUnit::createParameter(std::unique_ptr<t_Parameter> par)
{
    if (parameters && !parameters->state.isValid() && par.get() && !parameters->getParameter(par->paramID))
    {
        return parameters->createAndAddParameter(std::move(par));
    }

    return 0;
}

RangedAudioParameter *DspUnit::createParameter(const String &parId, const String &parName, const String &parLabel,
                                               const NormalisableRange<float> &range, float defaultValue,
                                               std::function<String(float)> valueToTextFunction,
                                               std::function<float(const String &)> textToValueFunction,
                                               bool isMetaParameter, bool isAutomatableParameter, bool isDiscrete,
                                               AudioProcessorParameter::Category category, bool isBoolean)
{
    if (parameters && !parameters->state.isValid() && !parameters->getParameter(parId))
    {
        return parameters->createAndAddParameter(std::make_unique<t_Parameter>(parId, parName, parLabel, range,
                                                                               defaultValue, valueToTextFunction,
                                                                               textToValueFunction, isMetaParameter,
                                                                               isAutomatableParameter, isDiscrete,
                                                                               category, isBoolean));
    }

    return 0;
}

RangedAudioParameter *DspUnit::getParameter(const String &parId) noexcept
{
    if (parameters)
    {
        return parameters->getParameter(parId);
    }

    return 0;
}

DspGui *DspUnit::createGui()
{
    if (activeGui)
    {
        return activeGui;
    }

    if (hasEditor())
    {
        DspGui *gui = getGuiType();

        if (gui)
        {
            ScopedLock(getCallbackLock());
            activeGui = gui;
        }

        return gui;
    }

    return 0;
}

ValueTree DspUnit::readFromData(const void *data, int sizeInBytes) const
{
    return ValueTree::readFromData(data, sizeInBytes);
}

void DspUnit::writeToMemory(ValueTree data, MemoryBlock &block) const
{
    MemoryOutputStream mos(block, false);

    if (data.isValid())
    {
        data.writeToStream(mos);
    }
}

//=====================================================================================================================
void DspUnit::initialize() noexcept
{
    this->setNonRealtime(processor.isNonRealtime());
    this->setProcessingPrecision(processor.getProcessingPrecision());
    this->setCurrentProgram(processor.getCurrentProgram());
    this->setLatencySamples(processor.getLatencySamples());
    this->setTypeOfNextNewPlugin(processor.wrapperType);
    this->setRateAndBufferSizeDetails(processor.getSampleRate(), processor.getBlockSize());
}
}
