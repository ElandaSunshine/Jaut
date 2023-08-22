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
    @file   jaut_AssertDef.h
    @date   21, March 2020
    
    ===============================================================
 */

#pragma once

#if !DOXYGEN

    // type-traits
    #define JAUT_ASSERT_TYPE_TRAITS_NO_TYPE_LIST "Type T must be a type with a type-list"
    #define JAUT_ASSERT_TYPE_TRAITS_OUT_OF_BOUNDS "Index out of bounds"
    #define JAUT_ASSERT_TYPE_TRAITS_DEFAULT_CASES "Only one default case is permitted, and it must appear at the end"
    #define JAUT_ASSERT_TYPE_TRAITS_NO_DEFAULT_CASES "None of the cases were met and no default case has been given"

    // jaut::Event
    #define JAUT_ASSERT_EVENT_NOT_A_HANDLER "Event has invalid template parameter, " \
                                            "parameter 'Handler' must be of type EventHandler"
    // jaut::Numeric
    #define JAUT_ASSERT_NUMERIC_TYPE_NOT_NUMERIC "The specified template parameter is not a numeric type"
    #define JAUT_ASSERT_NUMERIC_FAILSAFE_NOT_CHECK "Failsafe options must be valid NumericChecks constants"
    #define JAUT_ASSERT_NUMERIC_IEEE_NOT_SUPPORTED "Overflow checking of floating point values is only available " \
                                                   "for machines supporting the IEEE 754 standard"
    #define JAUT_ASSERT_NUMERIC_NOT_BIT_FLOATING "Floating-point types do not support bitwise operations"
    
    // jaut::SafeFloat
    #define JAUT_ASSERT_SAFEFLOAT_NO_FLOAT_TYPE "At least one type must be a floating point type"

    // jaut::NonNull
    #define JAUT_ASSERT_NONNULL_UNSUPPORTED_POINTER_TYPE
    
    // jaut::SerialisableJuceProcessorProxy
    #define JAUT_ASSERT_SERIALISABLE_JUCE_PROCESSOR_PROXY_NOT_AUDIO_PROCESSOR \
        "Type AudioProcessorClass must be of type juce::AudioProcessor"
    #define JAUT_ASSERT_SERIALISABLE_JUCE_PROCESSOR_PROXY_IS_ABSTRACT \
        "Type AudioProcessorClass cannot be an abstract class"

    // jaut::AudioProcessorSet
    #define JAUT_ASSERT_AUDIO_PROCESSOR_SET_NOT_I_SERIALISABLE_AUDIO_PROCESSOR \
        "Processors must inherit from jaut::SerializableAudioProcessor"
    #define JAUT_ASSERT_AUDIO_PROCESSOR_SET_NO_PROCESSORS \
        "Must have at least one processor"

    // jaut::AtomicRingBuffer
    #define JAUT_ASSERT_ATOMIC_RING_BUFFER_INVALID_CAPACITY "BufferSize must be at least 1"

    // jaut::Logger
    #define JAUT_ASSERT_LOGGER_OBJECT_NO_TOSTRING \
        "The given object is neither convertible to string nor does it have a 'toString()' method"
    #define JAUT_ASSERT_LOGGER_ROTATION_MANAGER_NO_SETUP \
        "No setup method with the given arguments found"
    #define JAUT_ASSERT_LOGGER_WORKER_NOT_A_WORKER \
        "Worker passed as template type is not an ILogWorker type"
    #define JAUT_ASSERT_LOGGER_WORKER_IS_ABSTRACT \
        "Worker passed as template type can not be an abstract class"

    // jaut::Stringable
    #define JAUT_ASSERT_STRINGABLE_NOT_CONVERTIBLE_TO_JUCE_STRING \
        "The given type is not convertible to juce::var"

    // jaut::ArgFilter
    #define JAUT_ASSERT_ARG_FILTER_NOT_SAME_ARG_LIST \
        "Arguments passed are different than from the jaut::ArgList template"
#endif
