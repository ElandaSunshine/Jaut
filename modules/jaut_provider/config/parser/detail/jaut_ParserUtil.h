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
    @file   jaut_ParserUtil.h
    @date   19, July 2022
    
    ===============================================================
 */

#pragma once

#include <jaut_provider/config/jaut_Config.h>
#include <jaut_provider/config/jaut_ConfigException.h>

namespace jaut::detail
{
    //==================================================================================================================
    struct Indent
    {
        //==============================================================================================================
        static juce::String makeIndent(int level, int indentWidth)
        {
            return Indent{ level, indentWidth }.toString();
        }
        
        //==============================================================================================================
        int depth;
        
        /** The width of one level of indent. */
        int width;
        
        //==============================================================================================================
        Indent next() const noexcept
        {
            return { depth + 1, width };
        }
        
        Indent prev() const noexcept
        {
            return { depth - 1, width };
        }
        
        juce::String toString(int depthMod = 0) const
        {
            const int amount = std::max(depth + depthMod, 0) * width;
            return (amount > 0 ? juce::String().paddedLeft(' ', amount) : "");
        }
    };
    
    //==================================================================================================================
    inline void throwWriteError(const juce::File &file)
    {
        const juce::String error_message = std::string(strerror(errno));
        throw jaut::ConfigIOException("config file could not be written, " + error_message + " ("
                                      + file.getFullPathName() + ")");
    }
    
    juce::String encodeJsonString(const juce::String &string)
    {
        juce::String::CharPointerType it = string.getCharPointer();
        juce::String output;
        output.preallocateBytes(string.getNumBytesAsUTF8() * JUCE_STRING_UTF_TYPE);
        
        for (;;)
        {
            const juce::juce_wchar c = *it;
            
            if (c == 0)
            {
                break;
            }
            
            switch (c)
            {
                case '\\': output << "\\\\"; break;
                case '"':  output << "\\\""; break;
                default: output << c;
            }
            
            ++it;
        }
        
        return output.replace("\n", "\\n");
    }
    
    inline juce::String serialiseJson(const juce::var &value, Indent indent)
    {
        switch (jaut::VarUtil::getVarType(value))
        {
            case jaut::VarUtil::VarTypeId::Void:
                return "null";
            
            case jaut::VarUtil::VarTypeId::String:
                return '\"' + encodeJsonString(value.toString()) + '\"';
            
            case jaut::VarUtil::VarTypeId::Double:
            case jaut::VarUtil::VarTypeId::Int:
            case jaut::VarUtil::VarTypeId::Int64:
                return value.toString();
            
            case jaut::VarUtil::VarTypeId::Bool:
                return (static_cast<bool>(value) ? "true" : "false");
            
            case jaut::VarUtil::VarTypeId::DynamicObject:
            {
                const juce::NamedValueSet &properties = value.getDynamicObject()->getProperties();
                
                if (properties.isEmpty())
                {
                    return "{}";
                }
                
                const juce::String indent_p1 = indent.toString(+1);
                juce::String       output    = juce::String("{") + juce::newLine;
                
                for (auto it = properties.begin(); it != properties.end(); ++it)
                {
                    const auto &[sub_value_id, sub_value] = *it;                    
                    output << indent_p1 << '\"' << sub_value_id << "\": "
                           << serialiseJson(sub_value, indent.next());
                    
                    if (std::next(it) != properties.end())
                    {
                        output << ',';
                    }
                    
                    output << juce::newLine;
                }
                
                return output + indent.toString() + "}";
            }
            
            case jaut::VarUtil::VarTypeId::Array:
            {
                const juce::Array<juce::var> &array = *value.getArray();
                
                if (array.isEmpty())
                {
                    return "[]";
                }
                
                const juce::String indent_p1 = indent.toString(+1);
                juce::String       output    = juce::String("[") + juce::newLine;
                
                for (auto it = array.begin(); it != array.end(); ++it)
                {
                    const juce::var &sub_value = *it;
                    output << indent_p1 << serialiseJson(sub_value, indent.next());
                    
                    if (std::next(it) != array.end())
                    {
                        output << ',';
                    }
                    
                    output << juce::newLine;
                }
                
                return output + indent.toString() + "]";
            }
            
            case jaut::VarUtil::VarTypeId::OtherObject:
            case jaut::VarUtil::VarTypeId::Undefined:
            case jaut::VarUtil::VarTypeId::Method:
            case jaut::VarUtil::VarTypeId::Unknown:
            case jaut::VarUtil::VarTypeId::BinaryData:
                break;
        }
        
        return juce::var::undefined();
    }
    
    inline void transformCategories(const jaut::Config::CategoryMap &src,
                                    std::vector<std::reference_wrapper<const jaut::Config::CategoryMap::value_type>>
                                        &dest,
                                    const juce::String &filter)
    {
        dest.reserve(src.size() - 1);
            
        for (const auto &pair : src)
        {
            if (pair.first != filter)
            {
                dest.emplace_back(pair);
            }
        }
    }
}
