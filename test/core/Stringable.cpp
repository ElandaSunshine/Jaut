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
#include <jaut_core/util/jaut_Stringable.h>

#include <jaut_core/util/jaut_VarUtil.cpp>



//**********************************************************************************************************************
// region Suite Setup
//======================================================================================================================
namespace
{
    
}
//======================================================================================================================
// endregion Suite Setup
//**********************************************************************************************************************
// region Unit Tests
//======================================================================================================================
TEST(StringableTestSuite, TestToString)
{
    // CPP
    EXPECT_EQ(jaut::toString(false), "false");
    EXPECT_EQ(jaut::toString(true),  "true");
    
    EXPECT_EQ(jaut::toString(nullptr), "null");
    
    EXPECT_EQ(jaut::toString(std::string_view("TEST")), "TEST");
    EXPECT_EQ(jaut::toString("TEST"), "TEST");
    
    // STD
    {
        std::array<std::string_view, 5> array { "Ha1", "Ha2", "Ha3", "Ha4", "Ha5" };
        EXPECT_EQ(jaut::toString(array), "[Ha1, Ha2, Ha3, Ha4, Ha5]");
    }
    
    {
        std::vector<std::string_view> vec { "Ha1", "Ha2", "Ha3", "Ha4", "Ha5" };
        EXPECT_EQ(jaut::toString(vec), "[Ha1, Ha2, Ha3, Ha4, Ha5]");
    }
    
    {
        std::deque<std::string_view> deq { "Ha1", "Ha2", "Ha3", "Ha4", "Ha5" };
        EXPECT_EQ(jaut::toString(deq), "[Ha1, Ha2, Ha3, Ha4, Ha5]");
    }
    
    {
        std::list<std::string_view> lst { "Ha1", "Ha2", "Ha3", "Ha4", "Ha5" };
        EXPECT_EQ(jaut::toString(lst), "[Ha1, Ha2, Ha3, Ha4, Ha5]");
    }
    
    {
        std::set<std::string_view> set { "Ha1", "Ha2", "Ha3", "Ha4", "Ha5" };
        EXPECT_EQ(jaut::toString(set), "[Ha1, Ha2, Ha3, Ha4, Ha5]");
    }
    
    {
        std::multiset<std::string_view> set { "Ha1", "Ha2", "Ha3", "Ha4", "Ha5" };
        EXPECT_EQ(jaut::toString(set), "[Ha1, Ha2, Ha3, Ha4, Ha5]");
    }
    
    {
        std::map<std::string_view, int> map {{"Ha1", 1}, {"Ha2", 2}, {"Ha3", 3}, {"Ha4", 4}, {"Ha5", 5}};
        EXPECT_EQ(jaut::toString(map), "{Ha1=1, Ha2=2, Ha3=3, Ha4=4, Ha5=5}");
    }
    
    {
        std::multimap<std::string_view, int> map {{"Ha1", 1}, {"Ha2", 2}, {"Ha3", 3}, {"Ha4", 4}, {"Ha5", 5}};
        EXPECT_EQ(jaut::toString(map), "{Ha1=1, Ha2=2, Ha3=3, Ha4=4, Ha5=5}");
    }
    
    // JUCE
    {
        juce::var val = juce::Array<juce::var>{ 0, 1, 2, 3, 4, 5 };
        EXPECT_EQ(jaut::toString(val), "[0, 1, 2, 3, 4, 5]");
        
        val = true;
        EXPECT_EQ(jaut::toString(val), "true");
        
        val = juce::var::undefined();
        EXPECT_EQ(jaut::toString(val), "undefined");
        
        val = juce::var();
        EXPECT_EQ(jaut::toString(val), "void");
        
        val = 0;
        EXPECT_EQ(jaut::toString(val), "0");
        
        val = "BLABLA";
        EXPECT_EQ(jaut::toString(val), "BLABLA");
    }
    
    {
        juce::StringArray test = juce::StringArray::fromLines("THIS\nA\nTEST\nHAHA\nOMEGAFUCKINGLUL");
        EXPECT_EQ(jaut::toString(test), "[THIS, A, TEST, HAHA, OMEGAFUCKINGLUL]");
    }
    
    {
        juce::HashMap<juce::String, int> map;
        map.set("Ha1", 1);
        map.set("Ha2", 2);
        map.set("Ha3", 3);
        map.set("Ha4", 4);
        map.set("Ha5", 5);
        
        EXPECT_EQ(jaut::toString(map), "{Ha1=1, Ha2=2, Ha3=3, Ha4=4, Ha5=5}");
    }
}
//======================================================================================================================
// endregion Unit Tests
//**********************************************************************************************************************
