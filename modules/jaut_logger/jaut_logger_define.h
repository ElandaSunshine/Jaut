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
    @file   jaut_logger_define.h
    @date   13, February 2023

    ===============================================================
 */
 
#pragma once



/** Config: JAUT_LOGGER_VERBOSE_TAG

    Specifies the tag for verbose log messages.
*/
#ifndef JAUT_LOGGER_VERBOSE_TAG
    #define JAUT_LOGGER_VERBOSE_TAG Verbose
#endif

/** Config: JAUT_LOGGER_ASYNC_SLEEP
    
    Specifies the sleep amount in milliseconds the async logging worker-thread will sleep for.
    Unless you know what you are doing, you shouldn't touch this.
    A value of -1 will disable the thread timeout and a value of 0 will yield the thread.
 */
#ifndef JAUT_LOGGER_ASYNC_SLEEP
    #define JAUT_LOGGER_ASYNC_SLEEP 100
#endif
