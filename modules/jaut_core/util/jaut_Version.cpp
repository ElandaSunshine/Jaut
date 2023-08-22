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
    @file   jaut_Version.cpp
    @date   20, April 2022
    
    ===============================================================
 */

#include <jaut_core/util/jaut_Version.h>

#include <regex>
#include <stdexcept>
#include <utility>

//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    bool matchesRegex(const std::string_view &patternInput, const juce::String &input)
    {
        const std::regex pattern(patternInput.data());
        return std::regex_match(input.toStdString(), pattern);
    }
    
    bool isNumericId(const juce::String &id) noexcept
    {
        juce::String::CharPointerType it = id.getCharPointer();
        
        for (;;)
        {
            const juce::juce_wchar c = it.getAndAdvance();
            
            if (c == 0)
            {
                return true;
            }
            
            if (!juce::CharacterFunctions::isDigit(c))
            {
                return false;
            }
        }
    }
    
    int asciiCompareId(const juce::String &left, const juce::String &right) noexcept
    {
        juce::String::CharPointerType lit = left.getCharPointer();
        juce::String::CharPointerType rit = right.getCharPointer();
        
        for (;;)
        {
            const juce::juce_wchar lc = lit.getAndAdvance();
            const juce::juce_wchar rc = rit.getAndAdvance();
    
            if (lc != rc)
            {
                return (lc > rc ? 1 : -1);
            }
            
            if (lc == 0)
            {
                return 0;
            }
        }
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Version
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    bool Version::isValidVersionString(const juce::String &version)
    {
        return ::matchesRegex(semVerFullPattern, version);
    }
    
    bool Version::isValidPreReleaseString(const juce::String &preReleaseString)
    {
        return ::matchesRegex(semVerPreReleasePattern, preReleaseString);
    }
    
    bool Version::isValidBuildString(const juce::String &buildString)
    {
        return ::matchesRegex(semVerBuildPattern, buildString);
    }
    
    //==================================================================================================================
    Version::Version(int parMajor, int parMinor, int parPatch, juce::String parPreRelease, juce::String parBuild)
        : preRelease(std::move(parPreRelease)), build(std::move(parBuild)),
          major(parMajor), minor(parMinor), patch(parPatch)
    {
        if (!preRelease.isEmpty() && !isValidPreReleaseString(preRelease))
        {
            throw std::runtime_error("invalid pre-release string '" + preRelease.toStdString() + "'");
        }
        
        if (!build.isEmpty() && !isValidBuildString(build))
        {
            throw std::runtime_error("invalid build string '" + build.toStdString() + "'");
        }
    }
    
    Version::Version(const juce::String &parVersion)
    {
        if (!parseString(parVersion))
        {
            throw std::runtime_error("invalid version string '" + parVersion.toStdString() + "'");
        }
    }
    
    //==================================================================================================================
    bool Version::operator==(const Version &right) const noexcept { return compare(right) ==  0; }
    bool Version::operator!=(const Version &right) const noexcept { return compare(right) !=  0; }
    bool Version::operator >(const Version &right) const noexcept { return compare(right) ==  1; }
    bool Version::operator>=(const Version &right) const noexcept { return compare(right)  > -1; }
    bool Version::operator <(const Version &right) const noexcept { return compare(right) == -1; }
    bool Version::operator<=(const Version &right) const noexcept { return compare(right)  <  1; }
    
    //==================================================================================================================
    juce::String Version::toString() const
    {
        juce::String version_string;
        version_string << major << '.' << minor << '.' << patch;
        
        if (!preRelease.isEmpty())
        {
            version_string << '-' << preRelease;
        }
        
        if (!build.isEmpty())
        {
            version_string << '+' << build;
        }
        
        return version_string;
    }
    
    //==================================================================================================================
    bool Version::parseString(const juce::String &versionString)
    {
        const std::regex  pattern(jaut::Version::semVerFullPattern.data());
        const std::string input = versionString.toStdString();
        std::smatch match;
        
        if (std::regex_match(input, match, pattern))
        {
            major = std::stoi(match.str(1));
            minor = std::stoi(match.str(2));
            patch = std::stoi(match.str(3));
            
            if (const auto g = match[4]; g.matched)
            {
                preRelease = g.str();
            }
            
            if (const auto g = match[5]; g.matched)
            {
                build = g.str();
            }
            
            return true;
        }
        
        return false;
    }
    
    int Version::compare(const Version &right) const noexcept
    {
        if (major != right.major)
        {
            return (major > right.major ? 1 : -1);
        }
        
        if (minor != right.minor)
        {
            return (minor > right.minor ? 1 : -1);
        }
        
        if (patch != right.patch)
        {
            return (patch > right.patch ? 1 : -1);
        }
        
        const bool pr_1 = preRelease.isEmpty();
        const bool pr_2 = right.preRelease.isEmpty();
        
        if (!pr_1 && !pr_2)
        {
            juce::StringArray ids_1;
            ids_1.addTokens(preRelease, ".", "");
            
            juce::StringArray ids_2;
            ids_2.addTokens(right.preRelease, ".", "");
            
            for (int i = 0;; ++i)
            {
                if (i == ids_1.size())
                {
                    return -static_cast<int>(i != ids_2.size());
                }
                
                if (i == ids_2.size())
                {
                    return static_cast<int>(i != ids_1.size());
                }
                
                const juce::String &id_1 = ids_1.getReference(i);
                const juce::String &id_2 = ids_2.getReference(i);
                
                const bool is_numeric_1 = ::isNumericId(id_1);
                const bool is_numeric_2 = ::isNumericId(id_2);
                
                if (is_numeric_1 != is_numeric_2)
                {
                    return (is_numeric_1 ? -1 : 1);
                }
                
                if (const int result = ::asciiCompareId(id_1, id_2))
                {
                    return result;
                }
            }
        }
        
        return (!pr_1 ? -1 : static_cast<int>(!pr_2));
    }
}
//======================================================================================================================
// endregion Version
//**********************************************************************************************************************
