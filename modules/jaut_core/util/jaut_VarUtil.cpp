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
    @file   jaut_VarUtil.cpp
    @date   08, June 2022
    
    ===============================================================
 */

#include "jaut_VarUtil.h"

namespace jaut
{
    VarUtil::VarTypeId VarUtil::getVarType(const juce::var &value) noexcept
    {
        if (value.isUndefined())  return VarTypeId::Undefined;
        if (value.isString())     return VarTypeId::String;
        if (value.isBool())       return VarTypeId::Bool;
        if (value.isInt())        return VarTypeId::Int;
        if (value.isInt64())      return VarTypeId::Int64;
        if (value.isDouble())     return VarTypeId::Double;
        if (value.isArray())      return VarTypeId::Array;
        if (value.isObject())     return (value.getDynamicObject() ? VarTypeId::DynamicObject : VarTypeId::OtherObject);
        if (value.isVoid())       return VarTypeId::Void;
        if (value.isMethod())     return VarTypeId::Method;
        if (value.isBinaryData()) return VarTypeId::BinaryData;
        
        return VarTypeId::Unknown;
    }
    
    std::string_view VarUtil::getTypeName(const juce::var &value) noexcept
    {
        static constexpr std::array names {
            "array",     "bool",   "dynamic object", "object",
            "double",    "int",    "int64",          "void",
            "undefined", "method", "binary data",    "string"
        };
        static_assert(names.size() == static_cast<std::size_t>(VarTypeId::Unknown));
        
        const VarTypeId type = getVarType(value);
        return (type != VarTypeId::Unknown ? names[static_cast<std::size_t>(type)] : "");
    }
}