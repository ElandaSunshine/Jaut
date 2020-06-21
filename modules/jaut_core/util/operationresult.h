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
    @file   operationresult.h
    @date   30, March 2020
    
    ===============================================================
 */

#pragma once

namespace jaut
{
/**
 *  A wrapper around a single int providing result information of an operation with a specific error code.
 */
class JAUT_API OperationResult
{
public:
    /**
     *  The result type of the operation.
     */
    enum JAUT_API Result
    {
        /** The operation went as planned. */
        Successful,
        
        /** The operation completely failed. */
        Failed
    };
    
    //==================================================================================================================
    /**
     *  Constructs a new OperationResult with state Ok.
     */
    OperationResult() noexcept = default;
    
    /**
     *  Constructs a new OperationResult from a boolean.
     *  @param result Ok if true, else error code -1
     */
    OperationResult(bool result) noexcept
        : result(result ? 0 : -1)
    {}
    
    /**
     *  Constructs a new OperationResult from an error code.
     *  @param errorCode The error code to store
     */
    OperationResult(int errorCode) noexcept
        : result(errorCode)
    {}
    
    //==================================================================================================================
    bool operator==(Result resultType) const noexcept
    {
        return resultType == Successful ? result == 0 : result != 0;
    }
    
    bool operator!=(Result resultType) const noexcept
    {
        return !(*this == resultType);
    }
    
    bool operator==(int errorCode) const noexcept
    {
        return result == errorCode;
    }
    
    bool operator!=(int errorCode) const noexcept
    {
        return result != errorCode;
    }
    
    //==================================================================================================================
    operator bool() const noexcept
    {
        return result == 0;
    }
    
    //==================================================================================================================
    /**
     *  Gets the error code this OperationResult represents.
     *  @return The error code
     */
    int getCode() const noexcept
    {
        return result;
    }
    
    /**
     *  Gets the result of the operation.
     *  @return The result
     */
    Result getResult() const noexcept
    {
        return result == 0 ? Successful : Failed;
    }
    
private:
    int result { 0 };
};
}
