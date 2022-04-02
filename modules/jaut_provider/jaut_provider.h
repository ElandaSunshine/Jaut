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
    @file   jaut_message.h
    @date   29, February 2020

    ===============================================================
    BEGIN_JUCE_MODULE_DECLARATION

    ID:                 jaut_provider
    vendor:             ElandaOfficial
    version:            0.1.0
    name:               JAUT Data & Storage Classes
    description:        Defines classes for advanced data management like configurations and folder management.
    website:            https://github.com/elandasunshine/jaut
    license:            AGPL v3
    minimumCppStandard: 17
    dependencies:       jaut_core juce_graphics

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

#if __has_include(<fmt/core.h>)
    #define JAUT_OPTLIB_FMT 1
#else
    #define JAUT_OPTLIB_FMT 0
#endif

#if __has_include(<yaml-cpp/yaml.h>)
    #define JAUT_OPTLIB_YAML_CPP 1
#else
    #define JAUT_OPTLIB_YAML_CPP 0
#endif

// Dependencies ========================================================================================================
#include <jaut_core/jaut_core.h>
#include <juce_graphics/juce_graphics.h>

// Exception ===========================================================================================================
#include <jaut_provider/exception/loggerexception.h>

// Config ==============================================================================================================
#include <jaut_provider/config/iconfigparser.h>
#include <jaut_provider/config/config.h>
#include <jaut_provider/config/configparser.h>

// Locale ==============================================================================================================
#include <jaut_provider/lang/localisation.h>

// Logger ==============================================================================================================
#include <jaut_provider/logger/iloggable.h>
#include <jaut_provider/logger/logmanager.h>
#include <jaut_provider/logger/logger.h>
#include <jaut_provider/logger/rotation/logrotationmanager.h>
#include "jaut_provider/logger/rotation/handler/disabledrotationhandler.h"
#include "jaut_provider/logger/rotation/handler/timedrotationhandler.h"
#include "jaut_provider/logger/rotation/handler/dailyrotationhandler.h"
#include "jaut_provider/logger/rotation/handler/sizelimitrotationhandler.h"
#include <jaut_provider/logger/sink/ilogsink.h>
#include <jaut_provider/logger/sink/sinkmixed.h>
#include <jaut_provider/logger/sink/detail/sinkcimpl.h>
#include <jaut_provider/logger/sink/sinkc.h>
#include <jaut_provider/logger/sink/detail/sinkfileimpl.h>
#include <jaut_provider/logger/sink/sinkfile.h>

#if JAUT_OPTLIB_FMT
    #include <jaut_provider/logger/sink/detail/sinkfmtimpl.h>
    #include <jaut_provider/logger/sink/sinkfmt.h>
#endif

// Themes ==============================================================================================================
#include <jaut_provider/theme/ithemedefinition.h>
#include <jaut_provider/theme/thememanager.h>

// Util ================================================================================================================
#include <jaut_provider/util/imetadata.h>
#include <jaut_provider/util/imetareader.h>
#include <jaut_provider/util/metadatautil.h>
