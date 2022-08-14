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

    @author Elanda (elanda@elandasunshine.com)
    @file   jaut_provider_define.h
    @date   08, May 2022

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

/** Config: JAUT_PROVIDER_STRICT_LANGUAGE_PROTOCOL

    Enables the strict language protocol which means that only languages that are officially stated in the ISO standard
    are valid names.
*/
#ifndef JAUT_PROVIDER_STRICT_LANGUAGE_PROTOCOL
    #define JAUT_PROVIDER_STRICT_LANGUAGE_PROTOCOL 0
#endif
