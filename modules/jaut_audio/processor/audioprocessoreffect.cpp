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
    @file   SfxUnit.cpp
    @date   23, December 2019
    
    ===============================================================
 */

#include <jaut/processor/sfxunit.h>

namespace
{
using namespace jaut;
using Parameter = AudioProcessorValueTreeState::Parameter;

inline String getParameterId(const String &id, const String &name, int index)
{
    return name + "_" + id + "_" + String(index);
}

inline AudioProcessorValueTreeState::Parameter *convertParameter(const SfxUnit::SfxParameter &parameter,
                                                                 const String &name, int index) noexcept
{
    const String par_name = name + " " + parameter.parameterName + " #" + String(index + 1);

    return new Parameter(getParameterId(parameter.parameterID.toLowerCase().removeCharacters(" "),
                                        par_name.toLowerCase().removeCharacters(" "), index),
                         parameter.parameterName, parameter.labelText, parameter.valueRange, parameter.defaultValue,
                         parameter.valueToText, parameter.textToValue, parameter.isMetaParameter,
                         parameter.isAutomatableParameter, parameter.isDiscrete, parameter.parameterCategory,
                         parameter.isBoolean);
}
}

namespace jaut
{
//======================================================================================================================
SfxUnit::SfxUnit(DspUnit &processor, AudioProcessorValueTreeState &vts, UndoManager *undoManager)
    : DspUnit(processor, &vts, undoManager)
{}

//======================================================================================================================
RangedAudioParameter *SfxUnit::getParameter(int index, const String &id)
{
    return parameters->getParameter(::getParameterId(id, getName(), index));
}

SfxUnit::DataContext *SfxUnit::getDataContext(int index)
{
    return contexts[index];
}

void SfxUnit::initialize()
{
    const auto sfx_parameters = createParameters();
    contexts.ensureStorageAllocated(getMaxInstances());

    for(int i = 0; i < getMaxInstances(); ++i)
    {
        // Parameters
        for(auto &parameter : sfx_parameters)
        {
            parameters->createAndAddParameter(std::unique_ptr<::Parameter>
                                              (::convertParameter(parameter, getName(), i)));
        }

        if(auto *context = getNewContext())
        {
            contexts.add(context);
        }
    }
}

//======================================================================================================================
void SfxUnit::process(AudioBuffer<float> &buffer,  MidiBuffer &midiBuffer)
{
    processEffect(0, buffer, midiBuffer);
}

void SfxUnit::process(AudioBuffer<double> &buffer, MidiBuffer &midiBuffer)
{
    processEffect(0, buffer, midiBuffer);
}

void SfxUnit::beginPlayback(double sampleRate, int bufferSize)
{
    beginPlayback(0, sampleRate, bufferSize);
}

void SfxUnit::finishPlayback()
{
    finishPlayback(0);
}
}
