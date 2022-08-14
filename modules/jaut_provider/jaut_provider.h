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
    dependencies:       jaut_core juce_events juce_graphics

    END_JUCE_MODULE_DECLARATION
    ===============================================================
 */

#pragma once

#include <jaut_provider/jaut_provider_define.h>

/*
#include <jaut_core/jaut_core.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>

#include <jaut_provider/exception/loggerexception.h>
#include <jaut_provider/config/jaut_IConfigParser.h>
#include <jaut_provider/config/jaut_Config.h>
#include <jaut_provider/config/jaut_ConfigProperty.h>
#include <jaut_provider/config/jaut_ConfigException.h>
#include <jaut_provider/config/parser/jaut_XmlParser.h>
#include <jaut_provider/config/parser/jaut_JsonParser.h>
#include <jaut_provider/config/parser/jaut_YamlParser.h>
#include <jaut_provider/lang/localisation.h>
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

#include <jaut_provider/theme/ithemedefinition.h>
#include <jaut_provider/theme/thememanager.h>
#include <jaut_provider/util/imetadata.h>
#include <jaut_provider/util/imetareader.h>
#include <jaut_provider/util/metadatautil.h>
*/