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
    @file   jaut_Stringable_juce_core.h
    @date   10, October 2022

    ===============================================================
 */
 
#pragma once

#include <jaut_core/util/jaut_VarUtil.h>



namespace jaut
{
    template<>
    struct JAUT_API Stringable<juce::var>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::var &object)
        {
            switch (VarUtil::getVarType(object))
            {
                case VarUtil::VarTypeId::Array:         return jaut::toString(*object.getArray());
                case VarUtil::VarTypeId::DynamicObject: return jaut::toString(*object.getDynamicObject());
                case VarUtil::VarTypeId::OtherObject:   return jaut::toString(*object.getObject());
                case VarUtil::VarTypeId::Bool:          return jaut::toString<bool>(object);
                
                case VarUtil::VarTypeId::Undefined: return "undefined";
                case VarUtil::VarTypeId::Void:      return "void";
                case VarUtil::VarTypeId::Method:    return "var(const NativeFunctionArgs&)";
                
                case VarUtil::VarTypeId::Double:
                case VarUtil::VarTypeId::Int:
                case VarUtil::VarTypeId::Int64:
                case VarUtil::VarTypeId::String:
                case VarUtil::VarTypeId::BinaryData:
                case VarUtil::VarTypeId::Unknown:
                    return object.toString();
            }
            
            return "";
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::var";
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::StringArray>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::StringArray &object)
        {
            return "[" + object.joinIntoString(", ") + "]";
        }
    
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::StringArray";
        }
    };
    
    template<class T, class CS, auto N>
    struct JAUT_API Stringable<juce::Array<T, CS, N>>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::Array<T, CS, N> &object)
        {
            juce::StringArray elements;
            elements.ensureStorageAllocated(object.size());
            
            for (const T &obj : object)
            {
                elements.add(jaut::toString(obj));
            }
            
            return '[' + elements.joinIntoString(", ") + ']';
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::Array<" + jaut::toName<T>() + '>';
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<juce::ArrayBase<T, Ect...>> : detail::SequencedImpl<juce::ArrayBase, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::ArrayBase<" + jaut::toName<T>() + '>';
        }
    };
    
    template<class T, class U, class ...Ect>
    struct JAUT_API Stringable<juce::HashMap<T, U, Ect...>>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::HashMap<T, U, Ect...> &object)
        {
            juce::StringArray elements;
            elements.ensureStorageAllocated(static_cast<int>(object.size()));
            
            for (juce::HashMap<juce::String, int>::Iterator i = object; i.next();)
            {
                elements.add(jaut::toString(i.getKey()) + "=" + jaut::toString(i.getValue()));
            }
            
            return '{' + elements.joinIntoString(", ") + '}';
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::HashMap<" + jaut::toName<T>() + ", " + jaut::toName<U>() + '>';
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<juce::OwnedArray<T, Ect...>> : detail::SequencedImpl<juce::OwnedArray, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::OwnedArray<" + jaut::toName<T>() + '>';
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::NamedValueSet>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::NamedValueSet &object)
        {
            juce::StringArray elements;
            elements.ensureStorageAllocated(object.size());
        
            for (const auto &[id, val] : object)
            {
                elements.add(id.toString() + "=" + jaut::toString(val));
            }
        
            return '{' + elements.joinIntoString(", ") + '}';
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::NamedValueSet";
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::StringPairArray>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::StringPairArray &object)
        {
            juce::StringArray elements;
            elements.ensureStorageAllocated(object.size());
        
            for (const juce::String &obj : object.getAllKeys())
            {
                elements.add(obj + "=" + object[obj]);
            }
        
            return '{' + elements.joinIntoString(", ") + '}';
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::StringPairArray";
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::PropertySet>
    {
        JAUT_NODISCARD
        static juce::String toString(juce::PropertySet &object)
        {
            return jaut::toString(object.getAllProperties());
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::PropertySet";
        }
    };
    
    template<class T, class ...Ect>
    struct JAUT_API Stringable<juce::SortedSet<T, Ect...>> : detail::SequencedImpl<juce::SortedSet, T, Ect...>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::SortedSet<" + jaut::toName<T>() + '>';
        }
    };
    
    template<class T>
    struct JAUT_API Stringable<juce::SparseSet<T>> : detail::SequencedImpl<juce::SparseSet, T>
    {
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::SparseSet<" + jaut::toName<T>() + '>';
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::DynamicObject>
    {
        JAUT_NODISCARD
        static juce::String toString(juce::DynamicObject &object)
        {
            return jaut::toString(object.getProperties());
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::DynamicObject";
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::File>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::File &object)
        {
            return object.getFullPathName();
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::File";
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::BigInteger>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::BigInteger &object)
        {
            return object.toString(10);
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::BigInteger";
        }
    };
    
    template<class T>
    struct JAUT_API Stringable<juce::Range<T>>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::Range<T> &object)
        {
            return '[' + object.getStart() + ".." + object.getEnd() + ']';
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::Range<" + jaut::toName<T>() + '>';
        }
    };
    
    template<class T>
    struct JAUT_API Stringable<juce::NormalisableRange<T>>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::NormalisableRange<T> &object)
        {
            return "{start="    + jaut::toString(object.start)          + ", "
                    "end="       + jaut::toString(object.end)           + ", "
                    "interval="  + jaut::toString(object.interval)      + ", "
                    "skew="      + jaut::toString(object.skew)          + ", "
                    "symmetric=" + jaut::toString(object.symmetricSkew) + "}";
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::NormalisableRange<" + jaut::toName<T>() + '>';
        }
    };
    
    template<class T>
    struct JAUT_API Stringable<juce::Atomic<T>>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::Atomic<T> &object)
        {
            return jaut::toString(object.get());
        }
    
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::Atomic<" + jaut::toName<T>() + '>';
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::ArgumentList::Argument>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::ArgumentList::Argument &object)
        {
            return object.text;
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::ArgumentList::Argument";
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::ArgumentList>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::ArgumentList &object)
        {
            juce::StringArray elements;
            elements.ensureStorageAllocated(object.size());
            
            for (int i = 0; i < object.size(); ++i)
            {
                elements.add(jaut::toString(object[i]));
            }
            
            return '[' + elements.joinIntoString(", ") + "]";
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::ArgumentList";
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::Uuid>
    {
        JAUT_NODISCARD
        static juce::String toString(const juce::Uuid &object)
        {
            return object.toDashedString();
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::Uuid";
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::URL>
    {
    private:
        static juce::String getParameterMap(const juce::StringArray &names, const juce::StringArray &values)
        {
            juce::String out;
            out.preallocateBytes(names.size() * 5);
            
            for (int i = 0; i < names.size(); ++i)
            {
                if (i < values.size())
                {
                    out << names.getReference(i) << "=" << values.getReference(i);
                }
                else
                {
                    out << names.getReference(i);
                }
                
                if (i < (names.size() - 1))
                {
                    out << ", ";
                }
            }
            
            return out;
        }
        
    public:
        JAUT_NODISCARD
        static juce::String toString(const juce::URL &object)
        {
            const juce::String parameter_list = getParameterMap(object.getParameterNames(),
                                                                object.getParameterValues());
            return "{url="       + object.toString(false) + ", "
                    "postData="  + object.getPostData()   + ", "
                    "parameters={" + parameter_list       + "}}";
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::URL";
        }
    };
    
    template<>
    struct JAUT_API Stringable<juce::SystemStats>
    {
    private:
        static juce::String getParameterMap(const juce::StringArray &names, const juce::StringArray &values)
        {
            juce::String out;
            out.preallocateBytes(names.size() * 5);
            
            for (int i = 0; i < names.size(); ++i)
            {
                if (i < values.size())
                {
                    out << names.getReference(i) << "=" << values.getReference(i);
                }
                else
                {
                    out << names.getReference(i);
                }
                
                if (i < (names.size() - 1))
                {
                    out << ", ";
                }
            }
            
            return out;
        }
        
        static juce::String getExtensionList()
        {
            using juce::SystemStats;
            
            juce::StringArray extensions;
            
            if (SystemStats::has3DNow())           extensions.add("3DNow");
            if (SystemStats::hasAVX())             extensions.add("AVX");
            if (SystemStats::hasAVX2())            extensions.add("AVX2");
            if (SystemStats::hasAVX512BW())        extensions.add("AVX512BW");
            if (SystemStats::hasAVX512CD())        extensions.add("AVX512CD");
            if (SystemStats::hasAVX512DQ())        extensions.add("AVX512DQ");
            if (SystemStats::hasAVX512ER())        extensions.add("AVX512ER");
            if (SystemStats::hasAVX512F())         extensions.add("AVX512F");
            if (SystemStats::hasAVX512IFMA())      extensions.add("AVX512IFMA");
            if (SystemStats::hasAVX512PF())        extensions.add("AVX512PF");
            if (SystemStats::hasAVX512VBMI())      extensions.add("AVX512VBMI");
            if (SystemStats::hasAVX512VPOPCNTDQ()) extensions.add("AVX512VPOPCNTDQ");
            if (SystemStats::hasAVX512VL())        extensions.add("hasAVX512VL");
            if (SystemStats::hasFMA3())            extensions.add("FMA3");
            if (SystemStats::hasFMA4())            extensions.add("FMA4");
            if (SystemStats::hasMMX())             extensions.add("MMX");
            if (SystemStats::hasNeon())            extensions.add("Neon");
            if (SystemStats::hasSSE())             extensions.add("SSE");
            if (SystemStats::hasSSE2())            extensions.add("SSE2");
            if (SystemStats::hasSSE3())            extensions.add("SSE3");
            if (SystemStats::hasSSE41())           extensions.add("SSE3");
            if (SystemStats::hasSSE42())           extensions.add("SSE3");
            if (SystemStats::hasSSSE3())           extensions.add("SSSE3");
            
            return extensions.joinIntoString(", ");
        }
        
    public:
        template<class U>
        JAUT_NODISCARD
        static juce::String toString(const juce::SystemStats&)
        {
            return toString();
        }
        
        JAUT_NODISCARD
        static juce::String toString()
        {
            using juce::SystemStats;
            
            const juce::String is64bit     = (SystemStats::isOperatingSystem64Bit() ? "true" : "false");
            const juce::String isSandboxed = (SystemStats::isRunningInAppExtensionSandbox() ? "true" : "false");
            
            juce::String out;
            out << "{computerName="  << SystemStats::getComputerName()          << ", "
                    "cpuModel="      << SystemStats::getCpuModel()              << ", "
                    "cpuVendor="     << SystemStats::getCpuVendor()             << ", "
                    "deviceDesc="    << SystemStats::getDeviceDescription()     << ", "
                    "deviceManu="    << SystemStats::getDeviceManufacturer()    << ", "
                    "displayLang="   << SystemStats::getDisplayLanguage()       << ", "
                    "user="          << SystemStats::getFullUserName()          << ", "
                    "juceVersion="   << SystemStats::getJUCEVersion()           << ", "
                    "logonName="     << SystemStats::getLogonName()             << ", "
                    "os="            << SystemStats::getOperatingSystemName()   << ", "
                    "deviceId="      << SystemStats::getUniqueDeviceID()        << ", "
                    "userLanguage="  << SystemStats::getUserLanguage()          << ", "
                    "userRegion="    << SystemStats::getUserRegion()            << ", "
                    "cpuSpeedMhz="   << SystemStats::getCpuSpeedInMegahertz()   << ", "
                    "memoryMb="      << SystemStats::getMemorySizeInMegabytes() << ", "
                    "logicalCores="  << SystemStats::getNumCpus()               << ", "
                    "physicalCores=" << SystemStats::getNumPhysicalCpus()       << ", "
                    "pageSize="      << SystemStats::getPageSize()              << ", "
                    "64bit="         << is64bit                                 << ", "
                    "sandboxed="     << isSandboxed                             << ", "
                    "extensions=["     << getExtensionList()                      << "]}";
        }
        
        JAUT_NODISCARD
        static juce::String name()
        {
            return "juce::SystemStats";
        }
    };
}
