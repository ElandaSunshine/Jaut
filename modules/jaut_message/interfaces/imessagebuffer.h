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
    @file   jaut_message.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

namespace jaut
{
// Forward declarations
class IMessage;

// Class declarations
/**
 *  The IMessageBuffer interface declares all needed virtual functions to craft your own message buffer.
 */
struct IMessageBuffer
{
    //==================================================================================================================
    /**
     *  A shared pointer type that holds an IMessage object.
     */
    using Message = std::shared_ptr<IMessage>;

    //==================================================================================================================
    /**
     *  Pushes a new Message onto the buffer.
     *  @param message The message to push
     */
    virtual void push(Message message) = 0;
    
    /**
     *  Pops a previously pushed message from the buffer and returns it.
     *  @return The popped message
     */
    virtual Message pop() = 0;
    
    //==================================================================================================================
    /**
     *  Determines whether the buffer is empty.
     *  @return True if empty, false if not
     */
    virtual bool isEmpty() const = 0;
    
    /**
     *  Determines whether the buffer is full.
     *  @return True if full, false if not
     */
    virtual bool isFull()  const = 0;
    
    //==================================================================================================================
    /**
     *  Gets the current amount of elements in the buffer.
     *  @return The number of pushed messages
     */
    virtual int size()     const = 0;
    
    /**
     *  Gets the absolute amount of elements this buffer can contain.
     *  @return The capacity of the buffer
     */
    virtual int capacity() const = 0;
};
}
