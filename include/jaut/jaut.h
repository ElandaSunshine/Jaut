/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   jaut.h
    @date   09, July 2019
    
    ===============================================================
    BEGIN_JUCE_MODULE_DECLARATION
 
    ID:            jaut
    vendor:        ElandaOfficial
    version:       1.0
    name:          JUCE Audio Utility Tools
    description:   A useful and simple DSP library for JUCE based projects.
    website:       https://github.com/elandaofficial/jaut
    license:       2019 ElandaOfficial. All rights reserved.
   
    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

#ifndef JAUT_INCLUDE_NAMESPACE
  #define JAUT_INCLUDE_NAMESPACE 1
#endif

#include <jaut/appdata.h>
#include <jaut/config.h>
#include <jaut/dspgui.h>
#include <jaut/dspunit.h>
#include <jaut/dspunitmanager.h>
#include <jaut/dynamicgui.h>
#include <jaut/exception.h>
#include <jaut/fontformat.h>
#include <jaut/guidspunitmanager.h>
#include <jaut/icomponentdrawing.h>
#include <jaut/iconfigparser.h>
#include <jaut/impls.h>
#include <jaut/ithemedefinition.h>
#include <jaut/localisation.h>
#include <jaut/metadatahelper.h>
#include <jaut/thememanager.h>

#if JAUT_INCLUDE_NAMESPACE
using namespace jaut; // @suppress("Using directive in header")
#endif
