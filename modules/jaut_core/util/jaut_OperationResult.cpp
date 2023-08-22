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
    @file   jaut_OperationResult.cpp
    @date   19, April 2022
    
    ===============================================================
 */

#include <jaut_core/util/jaut_OperationResult.h>

//**********************************************************************************************************************
// region OperationResult
//======================================================================================================================
namespace jaut
{
    //==================================================================================================================
    /**
        *  Constructs a new OperationResult from an error code.
        *  @param errorCode The error code to store
        */
    OperationResult::OperationResult(int parErrorCode, juce::String parMessage) noexcept
        : message(std::move(parMessage)),
          errorCode(parErrorCode)
    {}
    
    //==================================================================================================================
    bool OperationResult::success() const noexcept
    {
        return (getCode() == 0);
    }
    
    bool OperationResult::failed() const noexcept
    {
        return (getCode() != 0);
    }
    
    //==================================================================================================================
    OperationResult::operator bool() const noexcept
    {
        return success();
    }
    
    bool OperationResult::operator!() const noexcept
    {
        return failed();
    }
    
    //==================================================================================================================
    bool OperationResult::operator==(Result result) const noexcept
    {
        return (success() ? (result == Successful) : (result == Failed));
    }
    
    bool OperationResult::operator!=(Result result) const noexcept
    {
        return (!operator==(result));
    }
    
    //==================================================================================================================
    int OperationResult::getCode() const noexcept
    {
        return errorCode;
    }
    
    const juce::String& OperationResult::getMessage() const noexcept
    {
        return message;
    }
    
    OperationResult::Result OperationResult::getResult() const noexcept
    {
        return static_cast<Result>(failed());
    }
}
//======================================================================================================================
// endregion OperationResult
//**********************************************************************************************************************
