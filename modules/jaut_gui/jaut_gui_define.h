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
    @file   jaut_gui_define.h
    @date   08, March 2023

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>

#include <cstdint>



namespace jaut
{
    class LookAndFeel;
    
    JAUT_NODISCARD
    LookAndFeel& getDefaultLaf() noexcept;
}

#if !DOXYGEN
    #define JAUT_INTERN_CREATE_LAF() \
        LookAndFeel *lookAndFeel { &jaut::getDefaultLaf() }; \
        void checkAndSetLaf(juce::LookAndFeel *laf) noexcept; \
        void parentHierarchyChanged() noexcept override; \
        void lookAndFeelChanged() noexcept override;
    
    #define JAUT_INTERN_IMPL_LAF(CLASS) \
        void CLASS ::checkAndSetLaf(juce::LookAndFeel *parLaf) noexcept \
        {                               \
            if (LookAndFeel *laf = dynamic_cast<LookAndFeel*>(parLaf)) \
            { std::swap(lookAndFeel, laf); } \
        }\
        void CLASS ::parentHierarchyChanged() noexcept \
        { \
            checkAndSetLaf(&getParentComponent()->getLookAndFeel()); \
        } \
        \
        void CLASS ::lookAndFeelChanged() noexcept \
        { \
            checkAndSetLaf(&getLookAndFeel()); \
        }
    
    #define JAUT_INTERN_INIT_LAF() checkAndSetLaf(&getLookAndFeel())
#endif



/** Config: JAUT_GUI_COLOUR_NAMESPACE
    
    Specifies the JAUT namespace for colours, if for whatever reason you are not happy with the default, you can set it
    here to a different value.
    Do note though, that changing this, might make it incompatible in situations where colours are serialised and
    shared.
 */
#ifndef JAUT_GUI_COLOUR_NAMESPACE
    #define JAUT_GUI_COLOUR_NAMESPACE 0x02
#endif



namespace jaut
{
    /**
     *  Describes the composition of a colour id as used in JUCE.<br>
     *  This is a standard that was defined by JAUT and not by JUCE,
     *  because it fortunately just happens to conform to JUCE's colours.
     */
    struct JAUT_API Cid
    {
        /** The type of colour namespaces. */
        using NamespaceId = char;
        
        /** The type of partially qualified colour ids. */
        using ColourId = std::uint16_t;
        
        /** The type of fully qualified colour ids. */
        using FqColourId = int;
        
        /** A list of reserved colour namespaces. */
        enum Reserved : NamespaceId
        {
            /** This namespace can not be used by users, as it will only be handled internally. */
            Reserved = 0x00,
            
            /** The JUCE colour namespace. */
            Juce = 0x01,
            
            /** 
             *  The JAUT colour namespace.
             *  <br><br>
             *  This can be changed by the "JAUT_GUI_COLOUR_NAMESPACE" define, however,
             *  this is not recommended as it will impair compatibility when ids are collected by different means,
             *  namely serialisation purposes such as theming and layout files.
             */
            Jaut = JAUT_GUI_COLOUR_NAMESPACE
        };
    };
}
