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

    @author Elanda (elanda@elandasunshine.com)
    @file   jaut_LanguageException.h
    @date   11, August 2022

    ===============================================================
 */
 
#pragma once

#include <jaut_core/define/jaut_DefUtils.h>
#include <jaut_core/util/jaut_VarUtil.h>
#include <juce_core/juce_core.h>

#include <exception>

namespace jaut
{
    class LangIsoCodeException : public std::exception
    {
    public:
        static LangIsoCodeException fromLanguageCode(const juce::String &code)
        {
            return LangIsoCodeException("Invalid ISO 639-1 language code '" + code + "'");
        }
    
        static LangIsoCodeException fromCountryCode(const juce::String &code)
        {
            return LangIsoCodeException("Invalid ISO 3166-1 alpha-2 country code '" + code + "'");
        }
        
        //==============================================================================================================
        JAUT_NODISCARD
        const char* what() const noexcept override
        {
            return message.toRawUTF8();
        }
        
    private:
        explicit LangIsoCodeException(juce::String errorMessage) noexcept
            : message(std::move(errorMessage))
        {}
        
        //==============================================================================================================
        juce::String message;
    };
    
    class LangManagerIOException : public std::exception
    {
    public:
        explicit LangManagerIOException(juce::String errorMessage) noexcept
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
