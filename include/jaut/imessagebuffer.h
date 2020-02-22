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
    @file   message.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

#include <jaut/expo.h>

namespace jaut
{
class  IMessage;
struct IMessageBuffer
{
    using Message = std::shared_ptr<IMessage>;

    //==================================================================================================================
    virtual void push(Message message) = 0;
    virtual Message pop() = 0;
    virtual bool isEmpty() const = 0;
    virtual bool isFull()  const = 0;
    virtual int size()     const = 0;
    virtual int capacity() const = 0;
};
}
