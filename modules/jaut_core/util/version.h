/**
    ===============================================================
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
    
    @author Elanda
    @file   version.h
    @date   16, June 2020
    
    ===============================================================
 */
 
#pragma once

namespace jaut
{
/**
 *  A versioning helper that wraps around integers denoting the single fractions of a version string.
 *  This only handles numbers and no text such as "alpha", "beta", "rc" and so on.
 *
 *  A usual version looks like MAJOR.MINOR.PATCH and an optional BUILD number which won't be appended to the
 *  version string if it equals 0.
 *
 *  You can use this class to compare different versions and determine whether one version is smaller, bigger or equal.
 *  At the same time it deals with format checking and throws exceptions in the constructor if the format of the
 *  version is invalid.
 *
 *  <table>
 *      <caption>A short explanation of a version's individual numbers</caption>
 *      <tr>
 *          <th>Number</th>
 *          <th>Description</th>
 *      </tr>
 *      <tr>
 *          <td>MAJOR</td>
 *          <td>Determines feature and general changes in the software that are not backwards compatible anymore.</td>
 *      </tr>
 *      <tr>
 *          <td>MINOR</td>
 *          <td>Determines feature and general changes in the software that MUST BE backwards compatible.</td>
 *      </tr>
 *      <tr>
 *          <td>PATCH</td>
 *          <td>Determines bug fixes and patches in the software that MUST BE backwards compatible.</td>
 *      </tr>
 *      <tr>
 *          <td>BUILD</td>
 *          <td>Determines changes/fixes that require neither the MINOR nor the PATCH version to be incremented. </td>
 *      </tr>
 *  </table>
 */
class JAUT_API Version
{
public:
    /**
     *  Check if the given version string is in a valid version format.
     *
     *  @param version The version string to check
     *  @return True if the version string is in a valid format
     */
    static bool isValidVersionString(const juce::String &version)
    {
        if (version.containsOnly("0123456789.") &&
            (version.matchesWildcard("*.*.*", true) || version.matchesWildcard("*.*.*.*", true)))
        {
            juce::StringArray fractions;
            fractions.addTokens(version, ".", "\"");
        
            for (const auto &fraction : fractions)
            {
                if (fraction.startsWith("0"))
                {
                    return false;
                }
            }
            
            return true;
        }
        
        return false;
    }
    
    //==================================================================================================================
    /**
     *  Create a new Version object from numbers denoting its individual parts.
     *
     *  @param major The major version number
     *  @param minor The minor version number
     *  @param patch The patch version number
     *  @param build The build version number
     */
    Version(int major, int minor, int patch, int build = 0)
        : major(major), minor(minor), patch(patch), build(build)
    {
        if (major < 0 || minor < 0 || patch < 0)
        {
            const juce::String error = "Negative numbers are not allowed in versions. (" + toString() + ")";
            JAUT_THROWASSERT(error, exception::InvalidVersionFormatException);
        }
    }
    
    /**
     *  Tries to parse the string into a version object or throws an exception if in invalid format.
     *  @param version The version string in the format: MAJOR.MINOR.PATCH.BUILD (BUILD is optional)
     */
    explicit Version(const juce::String &version)
    {
        if (version.containsOnly("0123456789.") &&
            (version.matchesWildcard("*.*.*", false) || version.matchesWildcard("*.*.*.*", false)))
        {
            juce::StringArray fractions;
            fractions.addTokens(version, ".", "\"");
            
            for (int i = 0; i < fractions.size(); ++i)
            {
                const juce::String fraction = fractions[i];
                
                if (fraction.length() > 1 && fraction.startsWith("0"))
                {
                    const juce::String error = juce::String(VersionNames.at(static_cast<std::size_t>(i))) +
                                                            " version starts with a 0, this is invalid. (" +
                                                            version + ")";
                    JAUT_THROWASSERT(error, exception::InvalidVersionFormatException);
                }
            }
            
            major = fractions[0].getIntValue();
            minor = fractions[1].getIntValue();
            patch = fractions[2].getIntValue();
            
            if (fractions.size() == 4)
            {
                build = fractions[3].getIntValue();
            }
        }
        else
        {
            const juce::String error = "Invalid version string, the format must meet the requirements "
                                       "'MAJOR.MINOR.PATCH.BUILD'. (" + version + ")";
            JAUT_THROWASSERT(error, exception::InvalidVersionFormatException);
        }
    }
    
    //==================================================================================================================
    bool operator==(const Version &other) const noexcept
    {
        return major == other.major && minor == other.minor && patch == other.patch && build == other.build;
    }
    
    bool operator!=(const Version &other) const noexcept
    {
        return !(*this == other);
    }
    
    bool operator>(const Version &other) const noexcept
    {
        if (major != other.major)
        {
            return major > other.major;
        }
        else if (minor != other.minor)
        {
            return minor > other.minor;
        }
        else if (patch != other.patch)
        {
            return patch > other.patch;
        }
        else if (build != other.build)
        {
            return build > other.build;
        }
        
        return false;
    }
    
    bool operator>=(const Version &other) const noexcept
    {
        return *this == other || *this > other;
    }
    
    bool operator<(const Version &other) const noexcept
    {
        if (major != other.major)
        {
            return major < other.major;
        }
        else if (minor != other.minor)
        {
            return minor < other.minor;
        }
        else if (patch != other.patch)
        {
            return patch < other.patch;
        }
        else if (build != other.build)
        {
            return build < other.build;
        }
        
        return false;
    }
    
    bool operator<=(const Version &other) const noexcept
    {
        return *this == other || *this < other;
    }
    
    //==================================================================================================================
    /** Increases the major version number and resets all others to 0. */
    void increaseMajor() noexcept
    {
        ++major;
        minor = 0;
        patch = 0;
        build = 0;
    }
    
    /** Increases the minor version number and resets patch and build back to 0. */
    void increaseMinor() noexcept
    {
        ++minor;
        patch = 0;
        build = 0;
    }
    
    /** Increases the patch version number and resets build back to 0. */
    void increasePatch() noexcept
    {
        ++patch;
        build = 0;
    }
    
    /** Increases the build version number. */
    void increaseBuild() noexcept
    {
        ++build;
    }
    
    //==================================================================================================================
    /**
     *  Gets the major version number.
     *  Determines feature and general changes in the software that are not backwards compatible anymore.
     *
     *  @return The version number
     */
    int getMajor() const noexcept
    {
        return major;
    }
    
    /**
     *  Gets the minor version number.
     *  Determines feature and general changes in the software that MUST BE backwards compatible.
     *
     *  @return The version number
     */
    int getMinor() const noexcept
    {
        return minor;
    }
    
    /**
     *  Gets the patch version number.
     *  Determines bug fixes and patches in the software that MUST BE backwards compatible.
     *
     *  @return The version number
     */
    int getPatch() const noexcept
    {
        return patch;
    }
    
    /**
     *  Gets the build version number.
     *  Determines changes/fixes that require neither the MINOR nor the PATCH version to be incremented.
     *
     *  This is an optional part of the number.
     *
     *  @return The version number
     */
    int getBuild() const noexcept
    {
        return build;
    }
    
    //==================================================================================================================
    /**
     *  Determines whether the software is a release build or not.
     *  Usually release builds have a major version over 0, so this is used to determine whether it's a public build.
     *
     *  @return True if the version is a release version
     */
    bool isRelease() const noexcept
    {
        return major > 0;
    }
    
    //==================================================================================================================
    /**
     *  Returns the version this Version object holds in string representation.
     *  Note that if build equals 0, it will be omitted from the version string.
     *
     *  @return The version string
     */
    juce::String toString() const
    {
        juce::String version_string;
        version_string << major << "." << minor << "." << patch;
        
        if (build > 0)
        {
            version_string << "." << build;
        }
        
        return version_string;
    }
    
private:
    static constexpr std::array<const char*, 4> VersionNames { "Major", "Minor", "Patch", "Build" };
    
    //==================================================================================================================
    int major;
    int minor;
    int patch;
    int build { 0 };
};

inline bool operator==(const juce::String &versionString, const jaut::Version &version)
{
    const jaut::Version ver(versionString);
    return ver == version;
}

inline bool operator!=(const juce::String &versionString, const jaut::Version &version)
{
    const jaut::Version ver(versionString);
    return ver != version;
}

inline bool operator>(const juce::String &versionString, const jaut::Version &version)
{
    const jaut::Version ver(versionString);
    return ver > version;
}

inline bool operator<(const juce::String &versionString, const jaut::Version &version)
{
    const jaut::Version ver(versionString);
    return ver < version;
}

inline bool operator>=(const juce::String &versionString, const jaut::Version &version)
{
    const jaut::Version ver(versionString);
    return ver >= version;
}

inline bool operator<=(const juce::String &versionString, const jaut::Version &version)
{
    const jaut::Version ver(versionString);
    return ver <= version;
}
}
