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
    @file   atomicringbuffer.h
    @date   12, February 2020

    ===============================================================
 */

namespace jaut
{
template<int BufferSize = 64>
class JAUT_API AtomicRingBuffer final : public IMessageBuffer
{
public:
    std::array<Message, BufferSize> buffer;
    std::atomic<int> head;
    std::atomic<int> tail;

    //==============================================================================================================
    AtomicRingBuffer()
            : head(0),
              tail(0)
    {}

    //==============================================================================================================
    void push(Message message) override
    {
        const int current_head = head.load(std::memory_order_relaxed);
        const int next_head = (current_head + 1) % BufferSize;

        if (next_head != tail.load(std::memory_order_acquire))
        {
            buffer[current_head] = message;
            head.store(next_head, std::memory_order_release);
        }
    }

    //==============================================================================================================
    Message pop() override
    {
        const int current_tail = tail.load(std::memory_order_relaxed);

        if (current_tail == head.load(std::memory_order_acquire))
        {
            return nullptr;
        }

        Message message = std::exchange(buffer[current_tail], nullptr);
        tail.store((current_tail + 1) % BufferSize, std::memory_order_release);

        return message;
    }

    //==============================================================================================================
    int size() const override
    {
        const int current_head = head.load();
        const int current_tail = tail.load();

        return (current_head >= current_tail ? current_head - current_tail
                                             : current_head - current_tail + BufferSize);
    }

    int capacity() const override
    {
        return BufferSize;
    }

    //==============================================================================================================
    bool isFull() const override
    {
        return (((head + 1) % BufferSize) == tail);
    }

    bool isEmpty() const override
    {
        return (head == tail);
    }
};
}