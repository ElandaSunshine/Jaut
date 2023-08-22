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
    @file   jaut_ValueParameterAttachments.h
    @date   17, Septembre 2022

    ===============================================================
 */

#include <jaut_audio/parameter/jaut_ValueParameterAttachment.h>



//**********************************************************************************************************************
// region ValueParameterAttachment
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    ValueParameterAttachment::ValueParameterAttachment(juce::RangedAudioParameter &parParameter,
                                                       juce::Value                &parValueObject,
                                                       juce::UndoManager          *parUndoManager) noexcept
        : attachment(parParameter, [this](float value) { setValue(value); }, parUndoManager),
          valueObject(parValueObject)
    {
        sendInitialUpdate();
        valueObject.addListener(this);
    }
    
    ValueParameterAttachment::~ValueParameterAttachment()
    {
        valueObject.removeListener(this);
    }
    
    //==================================================================================================================
    void ValueParameterAttachment::sendInitialUpdate()
    {
        attachment.sendInitialUpdate();
    }
    
    //==================================================================================================================
    void ValueParameterAttachment::setValue(float parNewValue)
    {
        const juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);
        valueObject.setValue(parNewValue);
    }
    
    void ValueParameterAttachment::valueChanged(juce::Value &parValue)
    {
        if (ignoreCallbacks)
        {
            return;
        }
        
        attachment.beginGesture();
        attachment.setValueAsPartOfGesture(parValue.getValue());
        attachment.endGesture();
    }
}
//======================================================================================================================
// endregion ValueParameterAttachment
//**********************************************************************************************************************
