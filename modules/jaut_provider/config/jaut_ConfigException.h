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
    @file   jaut_ConfigException.h
    @date   26, April 2022

    ===============================================================
 */
 
#pragma once

#include <jaut_core/define/jaut_DefUtils.h>
#include <jaut_core/util/jaut_VarUtil.h>
#include <juce_core/juce_core.h>

#include <exception>

namespace jaut
{
    class ConfigPropertyIdException : public std::exception
    {
    public:
        ConfigPropertyIdException(const juce::String &id, const juce::String &problemMessage)
            : message("invalid property id '" + id + "', " + problemMessage)
        {}
        
        //==============================================================================================================
        JAUT_NODISCARD
        const char* what() const noexcept override
        {
            return message.toRawUTF8();
        }
        
    private:
        juce::String message;
    };
    
    class ConfigIncompatibleTypeException : public std::exception
    {
    public:
        struct ErrorCode
        {
            enum {
                /** Occurs when the parser doesn't support a certain type represented by juce::var */
                InvalidConversionType,
                
                /**
                 *  Occurs when the config has strict typing enabled and the parsed property type mismatches with the
                 *  property's type
                 */
                TypeMismatch,
                
                /**
                 *  Occurs when the config is set to strict typing but the property's default value has not been set
                 *  to a definite value type. (non-null)
                 */
                NoStrictType
            };
        };
    
        //==============================================================================================================
        JAUT_NODISCARD
        static ConfigIncompatibleTypeException invalidVarType(const juce::var &value)
        {
            const juce::String message = "couldn't write config, invalid var type '"
                                         + std::string(jaut::VarUtil::getTypeName(value)) + '\'';
            return ConfigIncompatibleTypeException(message, ErrorCode::InvalidConversionType);
        }
        
        JAUT_NODISCARD
        static ConfigIncompatibleTypeException unexpectedPropertyType(const juce::var &assigned,
                                                                      const juce::var &expected)
        {
            const juce::String message = juce::String("invalid property type '")
                                             + jaut::VarUtil::getTypeName(assigned).data()
                                         + "', expected '"
                                             + jaut::VarUtil::getTypeName(expected).data()
                                         + "' compatible type";
            return ConfigIncompatibleTypeException(message, ErrorCode::TypeMismatch);
        }
        
        JAUT_NODISCARD
        static ConfigIncompatibleTypeException needsStrictType(const juce::String &id)
        {
            const juce::String message = juce::String("invalid default value for id '" + id + "', in strict mode, "
                                                      "default values can neither be undefined nor void");
            return ConfigIncompatibleTypeException(message, ErrorCode::NoStrictType);
        }
        
        //==============================================================================================================
        JAUT_NODISCARD
        const char* what() const noexcept override
        {
            return message.toRawUTF8();
        }
        
        /** @see jaut::ConfigIncompatibleTypeException::ErrorCode */
        JAUT_NODISCARD
        int getCode() const noexcept
        {
            return code;
        }
        
    private:
        juce::String message;
        int          code;
        
        //==============================================================================================================
        explicit ConfigIncompatibleTypeException(juce::String errorMessage, int errorCode)
            : message(std::move(errorMessage)),
              code(errorCode)
        {}
    };
    
    class ConfigIOException : public std::exception
    {
    public:
        explicit ConfigIOException(juce::String errorMessage)
            : message(std::move(errorMessage))
        {}
        
        //==============================================================================================================
        JAUT_NODISCARD
        const char* what() const noexcept override
        {
            return message.toRawUTF8();
        }
        
    private:
        juce::String message;
    };
    
    class ConfigParseException : public std::exception
    {
    public:
        explicit ConfigParseException(juce::String errorMessage)
            : message(std::move(errorMessage))
        {}
        
        //==============================================================================================================
        JAUT_NODISCARD
        const char* what() const noexcept override
        {
            return message.toRawUTF8();
        }
        
    private:
        juce::String message;
    };
}
