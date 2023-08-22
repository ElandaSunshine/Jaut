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
    @file   jaut_CidNamespace.h
    @date   08, March 2023

    ===============================================================
 */

#pragma once

#include <jaut_gui/jaut_gui_define.h>

#include <jaut_core/define/jaut_Define.h>

#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    namespace detail
    {
        template<Cid::NamespaceId Id>
        struct JAUT_API CidNamespaceInternal
        {
            //==========================================================================================================
            /** The id of the colour namespace this type uses. */
            static constexpr Cid::NamespaceId namespaceId = Id;
            
            /** 
             *  The colour base for this namespace. (e.g. namespaceId: 0x01 => colourBase: 0x01000000)
             *  <br><br>
             *  The third hex number pair is reserved for future use,
             *  this means that partially qualified colour IDS can only be defined in the range of 0 to 65,535.
             *  (an unsigned 2-byte (16 bit) integer)
             */
            static constexpr Cid::FqColourId colourBase = (static_cast<Cid::FqColourId>(namespaceId) << (CHAR_BIT * 3));
            
            //==========================================================================================================
            /** 
             *  The resultant fully qualified colour id consisting of the namespace
             *  and the given partially qualified colour id.
             *  
             *  @tparam ColourId The partially qualified colour id
             */
            template<Cid::ColourId ColourId>
            static constexpr Cid::FqColourId getId = (colourBase | ColourId);
        };
    }
    
    /**
     *  Declares a unique namespace for colours of your library or app.
     *  <br><br>
     *  The best way to handle this is to make a type alias for your namespace and to use that to define your colour
     *  ids:
     *  @code
     *  // where "myId" = a value between 0x02 and 0x7F that doesn't clash with the JAUT defined namespace
     *  using MyCidNamespace = jaut::CidNamespace<{myId}>;
     *  @endcode
     *  
     *  It is also good practice to create a define for your namespace, such that in case another library defines the
     *  same namespace, this can be easily adjusted by the user:
     *  @code
     *  #ifndef MY_COLOUR_NAMESPACE
     *      // where "myId" = a value between 0x02 and 0x7F that doesn't clash with the JAUT defined namespace
     *      #define MY_COLOUR_NAMESPACE {myId}
     *  #endif
     *  
     *  using MyCidNamespace = jaut::CidNamespace<MY_COLOUR_NAMESPACE>;
     *  @endcode
     *  
     *  @tparam Id The id of the colour namespace
     */
    template<Cid::NamespaceId Id>
    struct JAUT_API CidNamespace : detail::CidNamespaceInternal<Id>
    {
        //==============================================================================================================
        static_assert((Id > Cid::Juce && Id != Cid::Jaut),
                      "A colour namespace ID cannot use any of the reserved namespaces and must be positive");
    };
    
    //==================================================================================================================
    /**
     *  The namespace for JUCE colour IDs.<br>
     *  Do not use this for defining new colour IDs.
     */
    using CidnsJuce = detail::CidNamespaceInternal<Cid::Juce>;
    
    /**
     *  The namespace for JAUT colour IDs.<br>
     *  Do not use this for defining new colour IDs.
     */
    using CidnsJaut = detail::CidNamespaceInternal<Cid::Jaut>;
}
