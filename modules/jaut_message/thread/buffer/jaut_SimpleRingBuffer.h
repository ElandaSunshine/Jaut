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
    @file   jaut_SimpleRingBuffer.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_AssertDef.h>

#include <jaut_core/define/jaut_Define.h>
#include <jaut_message/thread/message/jaut_IMessageBuffer.h>

#include <array>



namespace jaut
{
    //==================================================================================================================
    /**
     *  A simple queue implementation that allows pushing and popping from single threads.
     *  Only one thread can read and pop.
     *  <br><br>
     *  This is the single threaded variant and is not thread-safe, for a SPSC queue use jaut::AtomicRingBuffer.
     *  
     *  @tparam BufferSize How much space should be usable
     */
    template<int BufferSize, class T = IMessageBuffer<>::Message>
    class JAUT_API SimpleRingBuffer : public IMessageBuffer<T>
    {
    public:
        static_assert(BufferSize > 0, JAUT_ASSERT_ATOMIC_RING_BUFFER_INVALID_CAPACITY);
        
        //==============================================================================================================
        /**
         *  Since the way these buffers are handled, an additional slot must be allocated.
         *  This way BufferSize represents the actual usable storage and actualSize what is actually allocated.
         *  <br><br>
         *  This will always be "BufferSize + 1".
         */
        static constexpr int actualSize = (BufferSize + 1);
        
        //==============================================================================================================
        /** Constructs a new AtomicRingBuffer. */
        SimpleRingBuffer() noexcept = default;
        
        //==============================================================================================================
        int push(T message) override;
        
        JAUT_NODISCARD
        T pop() override;
        
        //==============================================================================================================
        JAUT_NODISCARD
        int size() const noexcept override;
        
        JAUT_NODISCARD
        int capacity() const noexcept override;
        
        //==============================================================================================================
        JAUT_NODISCARD
        bool isFull() const noexcept override;
        
        JAUT_NODISCARD
        bool isEmpty() const noexcept override;
        
    private:
        std::array<T, static_cast<std::size_t>(actualSize)> buffer;
        
        int head {};
        int tail {};
        
        //==============================================================================================================
        JAUT_NODISCARD
        int calculateSize(int head, int tail) const noexcept;
    };
    
    //==================================================================================================================
    template<int N, class T>
    inline int SimpleRingBuffer<N, T>::push(T parMessage)
    {
        if (isFull())
        {
            return -1;
        }
        
        std::swap(buffer[head], parMessage);
        head = (head + 1) % actualSize;
        
        return calculateSize(head, tail);
    }
    
    template<int N, class T>
    inline T SimpleRingBuffer<N, T>::pop()
    {
        if (isEmpty())
        {
            return T{};
        }
    
        T message;
        std::swap(message, buffer[tail]);
        tail = (tail + 1) % actualSize;
        
        return message;
    }
    
    //==================================================================================================================
    template<int N, class T>
    inline int SimpleRingBuffer<N, T>::size() const noexcept
    {
        return calculateSize(head, tail);
    }

    template<int N, class T>
    inline int SimpleRingBuffer<N, T>::capacity() const noexcept
    {
        return N;
    }

    //==================================================================================================================
    template<int N, class T>
    inline bool SimpleRingBuffer<N, T>::isFull() const noexcept
    {
        return (((head + 1) % actualSize) == tail);
    }
    
    template<int N, class T>
    inline bool SimpleRingBuffer<N, T>::isEmpty() const noexcept
    {
        return (head == tail);
    }
    
    //==================================================================================================================
    template<int N, class T>
    inline int SimpleRingBuffer<N, T>::calculateSize(int parHead, int parTail) const noexcept
    {
        return (parHead >= parTail ? (parHead - parTail) : (parHead - parTail + actualSize));
    }
}
