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
    @date   17, June 2020

    ===============================================================
 */
 
#pragma once

#include <jaut_core/define/jaut_Define.h>

#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_processors/juce_audio_processors.h>



namespace jaut
{
    /** A simple new parameter attachment type that allows attaching juce::Value objects to parameters. */
    class JAUT_API ValueParameterAttachment : private juce::Value::Listener
    {
    public:
        /**
         *  Creates a new value parameter attachment.
         *  
         *  @param parameter   The parameter to attach
         *  @param valueObject The value object to attach
         *  @param undoManager The optional undo manager to attach
         */
        ValueParameterAttachment(juce::RangedAudioParameter &parameter,
                                 juce::Value                &valueObject,
                                 juce::UndoManager          *undoManager = nullptr) noexcept;
        ~ValueParameterAttachment() override;
        
        //==============================================================================================================
        void sendInitialUpdate();
        
    private:
        juce::ParameterAttachment attachment;
        juce::Value &valueObject;
        bool ignoreCallbacks { false };
        
        //==============================================================================================================
        void setValue (float newValue);
        void valueChanged(juce::Value &value) override;
    };
}
