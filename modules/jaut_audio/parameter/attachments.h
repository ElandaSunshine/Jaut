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
    @file   attachments.h
    @date   17, June 2020

    ===============================================================
 */
 
#pragma once

namespace jaut
{
class ValueParameterAttachment : private juce::Value::Listener
{
public:
    ValueParameterAttachment(juce::RangedAudioParameter &parameter, juce::Value &valueObject,
                             juce::UndoManager *undoManager) noexcept
        : attachment(parameter, [this](float value) { setValue(value); }, undoManager),
          valueObject(valueObject)
    {
        sendInitialUpdate();
        valueObject.addListener(this);
    }
    
    ~ValueParameterAttachment() override
    {
        valueObject.removeListener(this);
    }
    
    //==================================================================================================================
    void sendInitialUpdate()
    {
        attachment.sendInitialUpdate();
    }
    
private:
    juce::ParameterAttachment attachment;
    juce::Value &valueObject;
    bool ignoreCallbacks { false };
    
    //==================================================================================================================
    void setValue (float newValue)
    {
        const juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);
        valueObject.setValue(newValue);
    }
    
    void valueChanged(juce::Value &value) override
    {
        if (ignoreCallbacks)
        {
            return;
        }
    
        attachment.beginGesture();
        attachment.setValueAsPartOfGesture(value.getValue());
        attachment.endGesture();
    }
};
}
