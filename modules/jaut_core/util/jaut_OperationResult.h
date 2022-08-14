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
    @file   jaut_OperationResult.h
    @date   30, March 2020
    
    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_Define.h>
#include <juce_core/juce_core.h>



namespace jaut
{
    //==================================================================================================================
    /** 
     *  A wrapper around an int and an optional error message providing result information of an operation
     *  with a specific error code.
     */
    class JAUT_API OperationResult
    {
    public:
        /** The result type of the operation. */
        enum JAUT_API Result
        {
            /** The operation went as planned. */
            Successful,
            
            /** The operation completely failed. */
            Failed
        };
        
        //==============================================================================================================
        /** Constructs a new OperationResult with state Ok. */
        OperationResult() noexcept = default;
        
        /**
         *  Constructs a new OperationResult from an error code and an optional error message.
         *  @param errorCode The error code
         *  @param message   The error message
         */
        OperationResult(int errorCode, juce::String message = "") noexcept; // NOLINT
        
        //==============================================================================================================
        /**
         *  Returns true if error-code is 0.
         *  @return Whether the operation was successful
         */
        JAUT_NODISCARD
        bool success() const noexcept;
        
        /**
         *  Returns true if error-code is non-zero.
         *  @return Whether the operation failed
         */
        JAUT_NODISCARD
        bool failed() const noexcept;
        
        //==============================================================================================================
        /**
         *  Gets whether this operation was a success or a failure.
         *  @return True if it was a success, false if not
         */
        JAUT_NODISCARD
        operator bool() const noexcept; // NOLINT
        
        /**
         *  Determines whether the result was unsuccessful.
         *  @return True if the operation failed, false if not
         */
        JAUT_NODISCARD
        bool operator!() const noexcept;
        
        //==============================================================================================================
        JAUT_NODISCARD
        bool operator==(Result result) const noexcept;
        
        JAUT_NODISCARD
        bool operator!=(Result result) const noexcept;
        
        //==============================================================================================================
        /**
         *  Gets the error code this OperationResult represents.
         *  @return The error code
         */
        JAUT_NODISCARD
        int getCode() const noexcept;
        
        /**
         *  Gets the optional error message.
         *  @return The error message
         */
        JAUT_NODISCARD
        const juce::String& getMessage() const noexcept;
        
        /**
         *  Gets the result of the operation.
         *  @return The result
         */
        JAUT_NODISCARD
        Result getResult() const noexcept;
        
    private:
        juce::String message;
        int          errorCode { 0 };
    };
}
