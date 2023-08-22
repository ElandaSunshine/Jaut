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
    @file   jaut_AtomicRingBuffer.h
    @date   12, February 2020

    ===============================================================
 */

#pragma once

#include <jaut_core/define/jaut_AssertDef.h>

#include <jaut_core/define/jaut_Define.h>
#include <jaut_message/thread/message/jaut_IMessageBuffer.h>

#include <array>
#include <atomic>



namespace jaut
{
    //==================================================================================================================
    /**
     *  A simple SPSC queue implementation that allows pushing and popping from two different threads.
     *  Only one thread can read and another can pop, but in a safe manner.
     *  <br><br>
     *  This is the thread-safe ring buffer variant, if you want a version that is single threaded you are better
     *  off with jaut::SimpleRingBuffer.
     *  
     *  @tparam BufferSize How much space should be usable
     */
    template<int BufferSize, class T = IMessageBuffer<>::Message>
    class JAUT_API AtomicRingBuffer : public IMessageBuffer<T>
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
        AtomicRingBuffer() noexcept = default;
        
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
        
        std::atomic<int> head {};
        std::atomic<int> tail {};
        
        //==============================================================================================================
        JAUT_NODISCARD
        int calculateSize(int head, int tail) const noexcept;
    };
    
    //==================================================================================================================
    template<int N, class T>
    inline int AtomicRingBuffer<N, T>::push(T parMessage)
    {
        const int current_head = head.load(std::memory_order_relaxed);
        const int next_head    = (current_head + 1) % actualSize;
    
        if (const int current_tail = tail.load(std::memory_order_acquire); next_head != current_tail)
        {
            std::swap(buffer[current_head], parMessage);
            head.store(next_head, std::memory_order_release);
            
            return calculateSize(current_head, current_tail);
        }
        
        return -1;
    }
    
    template<int N, class T>
    inline T AtomicRingBuffer<N, T>::pop()
    {
        const int current_tail = tail.load(std::memory_order_relaxed);
    
        if (current_tail == head.load(std::memory_order_acquire))
        {
            return T{};
        }
    
        T message;
        std::swap(message, buffer[current_tail]);
        
        tail.store((current_tail + 1) % actualSize, std::memory_order_release);
        
        return message;
    }
    
    //==================================================================================================================
    template<int N, class T>
    inline int AtomicRingBuffer<N, T>::size() const noexcept
    {
        const int current_head = head.load();
        const int current_tail = tail.load();
        return calculateSize(current_head, current_tail);
    }

    template<int N, class T>
    inline int AtomicRingBuffer<N, T>::capacity() const noexcept
    {
        return N;
    }

    //==================================================================================================================
    template<int N, class T>
    inline bool AtomicRingBuffer<N, T>::isFull() const noexcept
    {
        const int current_head = head.load();
        const int current_tail = tail.load();
        
        return (((current_head + 1) % actualSize) == current_tail);
    }
    
    template<int N, class T>
    inline bool AtomicRingBuffer<N, T>::isEmpty() const noexcept
    {
        const int current_head = head.load();
        const int current_tail = tail.load();
        
        return (current_head == current_tail);
    }
    
    //==================================================================================================================
    template<int N, class T>
    inline int AtomicRingBuffer<N, T>::calculateSize(int parHead, int parTail) const noexcept
    {
        return (parHead >= parTail
                ? (parHead - parTail)
                : (parHead - parTail + actualSize));
    }
}
