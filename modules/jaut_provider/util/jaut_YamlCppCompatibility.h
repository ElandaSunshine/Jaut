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

    Copyright (c) 2019 ElandaSunshine
    ===============================================================

    @author Elanda (elanda@elandasunshe.xyz)
    @file   jaut_YamlCppCompatibility.h
    @date   28, July 2022

    ===============================================================
 */

#pragma once

#include <jaut_core/util/jaut_VarUtil.h>
#include <jaut_provider/jaut_provider_define.h>
#include <juce_core/juce_core.h>

#include <optional>

#if JAUT_OPTLIB_YAML_CPP
    #include <yaml-cpp/yaml.h>



namespace YAML
{
    template <class T, class S>
    struct as_if<std::optional<T>, S>
    {
        //==============================================================================================================
        const Node& node;
        
        //==============================================================================================================
        explicit as_if(const Node &parNode)
            : node(parNode)
        {}
        
        //==============================================================================================================
        std::optional<T> operator()(const S& fallback) const
        {
            T out;
            
            if (node.m_pNode && convert<T>::decode(node, out))
            {
                return out;
            }
            
            return fallback;
        }
    };

    template <class T>
    struct as_if<std::optional<T>, void>
    {
        //==============================================================================================================
        const Node& node;
        
        //==============================================================================================================
        explicit as_if(const Node &parNode)
            : node(parNode)
        {}
        
        //==============================================================================================================
        std::optional<T> operator()() const
        {
            T out;
            
            if (node.m_pNode && convert<T>::decode(node, out))
            {
                return out;
            }
            
            throw TypedBadConversion<T>(node.Mark());
        }
    };
    
    //==================================================================================================================
    template<>
    struct convert<juce::String>
    {
        //==============================================================================================================
        static Node encode(const juce::String& rhs)
		{
			return Node(rhs.toStdString());
		}

		static bool decode(const Node& node, juce::String& rhs)
		{
            rhs = node.as<std::string>();
			return true;
		}
    };
    
    template<>
    struct convert<juce::Array<juce::var>>
    {
        //==============================================================================================================
        static Node encode(const juce::Array<juce::var>& rhs)
		{
			Node node(NodeType::Sequence);
            
            for (const auto &element : rhs)
            {
                node.push_back(element);
            }
            
            return node;
		}

		static bool decode(const Node& node, juce::Array<juce::var>& rhs)
		{
            if (!node.IsSequence())
            {
                return false;
            }
            
            rhs.ensureStorageAllocated(static_cast<int>(node.size()));
            
            for (const auto &e_node : node)
            {
                rhs.add(e_node.as<juce::var>());
            }
            
			return true;
		}
    };
    
    template<>
    struct convert<juce::NamedValueSet>
    {
        //==============================================================================================================
        static Node encode(const juce::NamedValueSet& rhs)
		{
			Node node(NodeType::Map);
            
            for (const auto &[id, obj] : rhs)
            {
                node[id.toString()] = obj;
            }
            
            return node;
		}

		static bool decode(const Node& node, juce::NamedValueSet& rhs)
		{
            if (!node.IsMap())
            {
                return false;
            }
            
            for (const auto &e_node : node)
            {
                rhs.set(e_node.first.as<juce::String>(), e_node.second.as<juce::var>());
            }
            
			return true;
		}
    };
        
    template<>
    struct convert<juce::var>
    {
        template<class First, class ...Remainder>
        static juce::var getOpt(const Node &node)
        {
            const auto opt = node.as<std::optional<First>>(std::nullopt);
            
            if (opt.has_value())
            {
                return opt.value();
            }
            
            if constexpr (sizeof...(Remainder) > 0)
            {
                return getOpt<Remainder...>(node);
            }
            else
            {
                return juce::var::undefined();
            }
        }
        
        static Node encode(const juce::var& rhs)
		{
			switch (jaut::VarUtil::getVarType(rhs))
            {
                case jaut::VarUtil::VarTypeId::String:        return Node(rhs.toString());
                case jaut::VarUtil::VarTypeId::Bool:          return Node(static_cast<bool>(rhs));
                case jaut::VarUtil::VarTypeId::Double:        return Node(static_cast<double>(rhs));
                case jaut::VarUtil::VarTypeId::Int:           return Node(static_cast<int>(rhs));
                case jaut::VarUtil::VarTypeId::Int64:         return Node(static_cast<juce::int64>(rhs));
                case jaut::VarUtil::VarTypeId::Void:          return Node(NodeType::Null);
                case jaut::VarUtil::VarTypeId::Array:         return Node(*rhs.getArray());
                case jaut::VarUtil::VarTypeId::DynamicObject: return Node(rhs.getDynamicObject()->getProperties());
                
                case jaut::VarUtil::VarTypeId::Undefined:
                case jaut::VarUtil::VarTypeId::OtherObject:
                case jaut::VarUtil::VarTypeId::Method:
                case jaut::VarUtil::VarTypeId::BinaryData:
                case jaut::VarUtil::VarTypeId::Unknown:
                    break;
            }
            
            return Node(NodeType::Undefined);
		}

		static bool decode(const Node& node, juce::var& rhs)
		{
			switch (node.Type())
            {
                case NodeType::Undefined: rhs = juce::var::undefined();            break;
                case NodeType::Null:      rhs = juce::var();                       break;
                case NodeType::Sequence:  rhs = node.as<juce::Array<juce::var>>(); break;
                
                case NodeType::Scalar:
                {
                    rhs = getOpt<int, juce::int64, double, bool, juce::String>(node);
                    break;
                }
                
                case NodeType::Map:
                {
                    auto d_obj = std::make_unique<juce::DynamicObject>();
                    d_obj->getProperties() = node.as<juce::NamedValueSet>();
                    rhs = d_obj.release();
                    
                    break;
                }
            }
            
            return true;
		}
    };
}
#endif
