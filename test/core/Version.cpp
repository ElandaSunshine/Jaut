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
    @file   Version.cpp
    @date   20, April 2022

    ===============================================================
 */

#include <gtest/gtest.h>
#include <jaut_core/util/jaut_Version.cpp>



//**********************************************************************************************************************
// region Suite Setup
//======================================================================================================================
namespace
{
    //==================================================================================================================
    constexpr std::size_t numTestVersions = 30;
    
    //==================================================================================================================
    using VersionArray = std::array<std::string_view, numTestVersions>;
    
    //==================================================================================================================
    // The tested version strings are from an online regex tester provided from the official semver document
    // They can be found here: https://regex101.com/r/vkijKf/1/
    constexpr VersionArray versionStrings {
        "0.0.4",
        "1.2.3",
        "10.20.30",
        "1.1.2-prerelease+meta",
        "1.1.2+meta",
        "1.1.2+meta-valid",
        "1.0.0-alpha",
        "1.0.0-beta",
        "1.0.0-alpha.beta",
        "1.0.0-alpha.beta.1",
        "1.0.0-alpha.1",
        "1.0.0-alpha0.valid",
        "1.0.0-alpha.0valid",
        "1.0.0-alpha-a.b-c-somethinglong+build.1-aef.1-its-okay",
        "1.0.0-rc.1+build.1",
        "2.0.0-rc.1+build.123",
        "1.2.3-beta",
        "10.2.3-DEV-SNAPSHOT",
        "1.2.3-SNAPSHOT-123",
        "1.0.0",
        "2.0.0",
        "1.1.7",
        "2.0.0+build.1848",
        "2.0.1-alpha.1227",
        "1.0.0-alpha+beta",
        "1.2.3----RC-SNAPSHOT.12.9.1--.12+788",
        "1.2.3----R-S.12.9.1--.12+meta",
        "1.2.3----RC-SNAPSHOT.12.9.1--.12",
        "1.0.0+0.build.1-rc.10000aaa-kk-0.1",
        "1.0.0-0A.is.legal"
    };
    
    constexpr VersionArray sortedVersionStrings {
        "10.20.30",
        "10.2.3-DEV-SNAPSHOT",
        "2.0.1-alpha.1227",
        "2.0.0",
        "2.0.0+build.1848",
        "2.0.0-rc.1+build.123",
        "1.2.3",
        "1.2.3-beta",
        "1.2.3-SNAPSHOT-123",
        "1.2.3----RC-SNAPSHOT.12.9.1--.12+788",
        "1.2.3----RC-SNAPSHOT.12.9.1--.12",
        "1.2.3----R-S.12.9.1--.12+meta",
        "1.1.7",
        "1.1.2+meta",
        "1.1.2+meta-valid",
        "1.1.2-prerelease+meta",
        "1.0.0",
        "1.0.0+0.build.1-rc.10000aaa-kk-0.1",
        "1.0.0-rc.1+build.1",
        "1.0.0-beta",
        "1.0.0-alpha0.valid",
        "1.0.0-alpha-a.b-c-somethinglong+build.1-aef.1-its-okay",
        "1.0.0-alpha.beta.1",
        "1.0.0-alpha.beta",
        "1.0.0-alpha.0valid",
        "1.0.0-alpha.1",
        "1.0.0-alpha",
        "1.0.0-alpha+beta",
        "1.0.0-0A.is.legal",
        "0.0.4"
    };
    
    constexpr VersionArray invalidVersionStrings {
        "1",
        "1.2",
        "1.2.3-0123",
        "1.2.3-0123.0123",
        "1.1.2+.123",
        "+invalid",
        "-invalid+invalid",
        "-invalid.01",
        "alpha",
        "alpha.beta",
        "alpha.beta.1",
        "alpha.1",
        "alpha+beta",
        "alpha_beta",
        "alpha.",
        "beta",
        "1.0.0-alpha_beta",
        "-alpha.",
        "1.0.0-alpha..",
        "1.0.0-alpha..1",
        "1.2",
        "1.2.3.DEV",
        "1.2-SNAPSHOT",
        "1.2.31.2.3----RC-SNAPSHOT.12.09.1--..12+788",
        "1.2-RC-SNAPSHOT",
        "-1.0.3-gamma+b7718",
        "+justmeta",
        "9.8.7+meta+meta",
        "9.8.7-whatever+meta+meta",
        "99999999999999999999999.999999999999999999.99999999999999999----RC-SNAPSHOT.12.09.1--------------------------------..12"
    };
    
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TEST(VersionTestSuite, TestSemverComparison)
{
    jaut::Version version1("2.0.0");
    jaut::Version version2("1.1.0");
    EXPECT_TRUE (version1 > version2);
    EXPECT_TRUE (version2 < version1);
    EXPECT_TRUE (version1 >= version2);
    EXPECT_TRUE (version2 <= version1);
    EXPECT_TRUE (version1 != version2);
    EXPECT_FALSE(version1 == version2);
    
    version1 = jaut::Version("1.0.0+test");
    version2 = jaut::Version("1.0.0");
    EXPECT_FALSE(version1 > version2);
    EXPECT_FALSE(version2 < version1);
    EXPECT_TRUE (version1 >= version2);
    EXPECT_TRUE (version2 <= version1);
    EXPECT_FALSE(version1 != version2);
    EXPECT_TRUE (version1 == version2);
    
    version1 = jaut::Version("1.0.0-prerelease");
    version2 = jaut::Version("1.0.0");
    EXPECT_TRUE (version1 < version2);
    EXPECT_TRUE (version2 > version1);
    EXPECT_TRUE (version2 >= version1);
    EXPECT_TRUE (version1 <= version2);
    EXPECT_TRUE (version1 != version2);
    EXPECT_FALSE(version1 == version2);
}

TEST(VersionTestSuite, TestSemverPrecedence)
{
    VersionArray result = ::versionStrings;
    
    ASSERT_NO_THROW
    (
        std::stable_sort(result.begin(), result.end(), [](const std::string_view &left, const std::string_view &right)
        {
            return (jaut::Version(std::string(left)) > jaut::Version(std::string(right)));
        });
    );
    
    ASSERT_EQ(result, ::sortedVersionStrings);
}

TEST(VersionTestSuite, TestSemverInvalidStrings)
{
    for (const auto &version : ::invalidVersionStrings)
    {
        EXPECT_THROW
        ({
            const jaut::Version version_obj((std::string(version)));
        }, std::runtime_error);
    }
}
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
