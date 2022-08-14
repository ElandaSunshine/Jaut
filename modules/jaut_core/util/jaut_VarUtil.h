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
    @file   jaut_VarUtil.h
    @date   08, June 2022
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/util/jaut_TypeContainer.h>

#include <juce_core/juce_core.h>


namespace jaut
{
    class JAUT_API VarUtil
    {
    public:
        enum class VarTypeId
        {
            Array,
            Bool,
            DynamicObject,
            OtherObject,
            Double,
            Int,
            Int64,
            Void,
            Undefined,
            Method,
            BinaryData,
            String,
            // FUTURE, probable but less-likely, juce::var type additions
            Unknown
        };
        
        struct Undefined
        {
            inline static juce::var value = juce::var::undefined();
        };
        
        struct Unknown {};

    private:
        using VarTypeArray = TypeArray<
            juce::Array<juce::var>, bool, juce::DynamicObject, juce::ReferenceCountedObject, double, int, juce::int64,
            void, Undefined, juce::var::NativeFunction, juce::MemoryBlock, juce::String
        >;
    
        static_assert(VarTypeArray::length == static_cast<std::size_t>(VarTypeId::Unknown));

    public:
        /**
         *  Gets the type of the juce::var object depending on the type-id.
         *  @tparam Id The juce::var type id
         *
         *  @see jaut::VarUtil::VarTypeId
         */
        template<VarTypeId Id>
        using VarType_t = VarTypeArray::at<static_cast<std::size_t>(Id)>;
        
        //==============================================================================================================
        /**
         *  Gets a named type for a var object.
         *  @param value The var object to read
         *  @return The enum name of the type of the object
         */
        JAUT_NODISCARD
        static VarTypeId getVarType(const juce::var &value) noexcept;
        
        /**
         *  Gets a string name representing the type of the var object.
         *  @param value The var object to read
         *  @return The string name of the type of the object
         */
        JAUT_NODISCARD
        static std::string_view getTypeName(const juce::var &value) noexcept;
    };
}