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
    @file   loggerexception.h
    @date   29, March 2022

    ===============================================================
 */
 
#pragma once

namespace jaut
{
    class LoggerExistsException final : public std::exception
    {
    public:
        explicit LoggerExistsException(const juce::String &name)
            : message("'" + name + "' already exists")
        {}
        
        //==============================================================================================================
        const char* what() const noexcept override
        {
            return message.toRawUTF8();
        }
        
    private:
        juce::String message;
    };
    
    class NoSuchLoggerException final : public std::exception
    {
    public:
        explicit NoSuchLoggerException(const juce::String &name)
                : message("'" + name + "' does not exist")
        {}
        
        //==============================================================================================================
        const char* what() const noexcept override
        {
            return message.toRawUTF8();
        }
    
    private:
        juce::String message;
    };
    
    class LogRotationException final : public std::exception
    {
    public:
        explicit LogRotationException(juce::String error)
            : message(std::move(error))
        {}
    
        //==============================================================================================================
        const char* what() const noexcept override
        {
            return message.toRawUTF8();
        }

    private:
        juce::String message;
    };
    
    class LogIOException final : public std::exception
    {
    public:
        explicit LogIOException(juce::String error)
        : message(std::move(error))
        {}
    
        //==============================================================================================================
        const char* what() const noexcept override
        {
            return message.toRawUTF8();
        }

    private:
        juce::String message;
    };
}