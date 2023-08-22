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
    @file   version.h
    @date   16, June 2020
    
    ===============================================================
 */
 
#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <jaut_core/util/jaut_Stringable.h>

#include <juce_core/juce_core.h>

#include <optional>
#include <string_view>

namespace jaut
{
    /**
     *  A versioning helper that wraps around different versioning components.<br>
     *  <br>
     *  To learn more about the format being used, please refer to the official semver paper:
     *  https://semver.org/#is-there-a-suggested-regular-expression-regex-to-check-a-semver-string<br>
     *  <br>
     *  You can use this class to compare different versions and determine whether one version is smaller,
     *  bigger or equal.
     *  At the same time it deals with format checking and throws exceptions if the format of the version is invalid.
     *  
     *  <table>
     *      <caption>A short explanation of a version's individual components</caption>
     *      <tr>
     *          <th>Name</th>
     *          <th>Description</th>
     *      </tr>
     *      <tr>
     *          <td>Major</td>
     *          <td>
     *              Determines API changes in the software that are not needed to be backwards compatible anymore.
     *          </td>
     *      </tr>
     *      <tr>
     *          <td>Minor</td>
     *          <td>Determines changes in the software that MUST BE backwards compatible.</td>
     *      </tr>
     *      <tr>
     *          <td>Patch</td>
     *          <td>Determines bug fixes and patches in the software that MUST BE backwards compatible.</td>
     *      </tr>
     *      <tr>
     *          <td>Pre-Release&nbsp;&nbsp;</td>
     *          <td>
     *              Determines the release-type of a software, this can be any changes.
     *          </td>
     *      </tr>
     *      <tr>
     *          <td>Build</td>
     *          <td>
     *              Determines the build version of the software. This may or may not mean that changes have been made.
     *          </td>
     *      </tr>
     *  </table><br>
     *  <br>
     *  Please note that, while semver technically allows version numbers to be of any magnitude, for efficiency reasons
     *  the limits of int apply here. (you wouldn't need such horrendously high version numbers anyway)
     *  If the numeric limits of int are crossed, a std::out_of_range exception will be thrown.
     */
    class JAUT_API Version
    {
    public:
        /** The official "Semantic Versioning 2.0.0" regex to match version strings. */
        static constexpr std::string_view semVerFullPattern =
            // major.minor.patch
            R"PAT((0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*))PAT"
            
            // pre-release label (-id.id...)
            R"PAT((?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?)PAT"
            
            // build label (+id.id...)
            R"PAT((?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?)PAT";
        
        static constexpr std::string_view semVerMajorMinorPatchPattern = semVerFullPattern.substr(  0, 40);
        static constexpr std::string_view semVerPreReleasePattern      = semVerFullPattern.substr( 40, 95);
        static constexpr std::string_view semVerBuildPattern           = semVerFullPattern.substr(135);
        
        /**
         *  Check if the given version string is in a valid version format.
         *  
         *  @param version The version string to check
         *  @return True if the version string is in a valid format
         */
        JAUT_NODISCARD static bool isValidVersionString(const juce::String &version);
        
        /**
         *  Check if the given part is a "valid pre-release" string.
         *  
         *  @param version The version string to check
         *  @return True if the version string is in a valid format
         */
        JAUT_NODISCARD static bool isValidPreReleaseString(const juce::String &preReleaseString);
        
        /**
         *  Check if the given part is a "build" string.
         *  
         *  @param version The version string to check
         *  @return True if the version string is in a valid format
         */
        JAUT_NODISCARD static bool isValidBuildString(const juce::String &buildString);
        
        //==============================================================================================================
        /** The pre-release identifier. */
        juce::String preRelease;
        
        /** The build identifier. */
        juce::String build;
        
        /** The major version number. */
        int major { 0 };
        
        /** The minor version number. */
        int minor { 1 };
        
        /** The patch version number. */
        int patch { 0 };
        
        //==============================================================================================================
        /** Creates a version object with the recommended initial version. (0.1.0) */
        Version() noexcept = default;
        
        /**
         *  Create a new Version object from numbers and an optional label
         *  
         *  @param major      The major version number
         *  @param minor      The minor version number
         *  @param patch      The patch version number
         *  @param preRelease The optional pre-release label
         *  @param build      The optional build label
         */
        Version(int major, int minor, int patch = 0, juce::String preRelease = "", juce::String build = "");
        
        /**
         *  Tries to parse the string into a version object or throws an exception if in invalid format.
         *  @param version The version string
         */
        explicit Version(const juce::String &version);
        
        //==============================================================================================================
        /**
         *  Determines whether this and another version object are equal.
         *  
         *  The way this works is by piecewise comparison, first checks major differences.
         *  If there are differences, the result can already be evaluated, otherwise goes on to the next part of
         *  the version.
         *  
         *  For the labeled pre-release and build parts, this is a bit trickier.
         *  It's best to consult the official "Semantic Versioning 2.0.0" documentation found here:
         *  https://semver.org/#spec-item-11
         *  
         *  @param right The right version to compare against 
         *  @return True if both are the same, otherwise false
         */
        JAUT_NODISCARD bool operator==(const Version &right) const noexcept;
        
        /**
         *  Determines whether this and another version object are different.
         *  
         *  The way this works is by piecewise comparison, first checks major differences.
         *  If there are differences, the result can already be evaluated, otherwise goes on to the next part of
         *  the version.
         *  
         *  For the labeled pre-release and build parts, this is a bit trickier.
         *  It's best to consult the official "Semantic Versioning 2.0.0" documentation found here:
         *  https://semver.org/#spec-item-11
         *  
         *  @param right The right version to compare against 
         *  @return True if both are different, otherwise false
         */
        JAUT_NODISCARD bool operator!=(const Version &right) const noexcept;
        
        /**
         *  Determines whether this version object contains a higher version than right.
         *  
         *  The way this works is by piecewise comparison, first checks major differences.
         *  If there are differences, the result can already be evaluated, otherwise goes on to the next part of
         *  the version.
         *  
         *  For the labeled pre-release and build parts, this is a bit trickier.
         *  It's best to consult the official "Semantic Versioning 2.0.0" documentation found here:
         *  https://semver.org/#spec-item-11
         *  
         *  @param right The right version to compare against 
         *  @return True if this version is higher
         */
        JAUT_NODISCARD bool operator>(const Version &right) const noexcept;
        
        /**
         *  Determines whether this version object contains a higher or equal version to right.
         *  
         *  The way this works is by piecewise comparison, first checks major differences.
         *  If there are differences, the result can already be evaluated, otherwise goes on to the next part of
         *  the version.
         *  
         *  For the labeled pre-release and build parts, this is a bit trickier.
         *  It's best to consult the official "Semantic Versioning 2.0.0" documentation found here:
         *  https://semver.org/#spec-item-11
         *  
         *  @param right The right version to compare against 
         *  @return True if this version is higher or equal
         */
        JAUT_NODISCARD bool operator>=(const Version &right) const noexcept;
        
        /**
         *  Determines whether this version object contains a lower version than right.
         *  
         *  The way this works is by piecewise comparison, first checks major differences.
         *  If there are differences, the result can already be evaluated, otherwise goes on to the next part of
         *  the version.
         *  
         *  For the labeled pre-release and build parts, this is a bit trickier.
         *  It's best to consult the official "Semantic Versioning 2.0.0" documentation found here:
         *  https://semver.org/#spec-item-11
         *  
         *  @param right The right version to compare against 
         *  @return True if this version is lower
         */
        JAUT_NODISCARD bool operator<(const Version &right) const noexcept;
        
        /**
         *  Determines whether this version object contains a lower or equal version to right.
         *  
         *  The way this works is by piecewise comparison, first checks major differences.
         *  If there are differences, the result can already be evaluated, otherwise goes on to the next part of
         *  the version.
         *  
         *  For the labeled pre-release and build parts, this is a bit trickier.
         *  It's best to consult the official "Semantic Versioning 2.0.0" documentation found here:
         *  https://semver.org/#spec-item-11
         *  
         *  @param right The right version to compare against 
         *  @return True if this version is lower or equal
         */
        JAUT_NODISCARD bool operator<=(const Version &right) const noexcept;
        
        //==============================================================================================================
        /**
         *  Returns the version this Version object holds as string.
         *  @return The version string
         */
        JAUT_NODISCARD juce::String toString() const;
        
    private:
        JAUT_NODISCARD bool parseString(const juce::String &input);
        JAUT_NODISCARD int compare(const Version &right) const noexcept;
    };
    
    //==================================================================================================================
    template<>
    struct JAUT_API Stringable<Version>
    {
        JAUT_NODISCARD
        static juce::String toString(const Version &object)
        {
            return object.toString();
        }
    };
}
