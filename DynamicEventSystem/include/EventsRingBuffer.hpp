#pragma once

#include <array>
#include <cstddef>
#include <cstring>

#include "typedefs.hpp"

namespace event_system {
namespace dynamic {

template <std::size_t Capacity>
class EventsRingBuffer
{
public:
    struct Reservation
    {
        std::size_t payloadOffset{0};
        std::size_t payloadSize{0};
        std::size_t reservedBytes{0};
    };

    bool tryWrite(const EventBufferView view, const std::size_t alignment,
                  Reservation& reservation)
    {
        if (view.data == nullptr || view.size == 0)
        {
            return false;
        }

        const std::size_t safeAlignment = normalizeAlignment(alignment);
        const std::size_t available = Capacity - m_used;
        if (view.size > available)
        {
            return false;
        }

        const std::size_t head = m_head;
        const std::size_t tail = m_tail;

        std::size_t payloadOffset = 0;
        std::size_t padding = 0;

        if (head >= tail)
        {
            const std::size_t alignedHead = alignUp(head, safeAlignment);
            if (alignedHead + view.size <= Capacity)
            {
                payloadOffset = alignedHead;
                padding = alignedHead - head;
            }
            else
            {
                padding = Capacity - head;
                const std::size_t wrappedOffset = alignUp(0, safeAlignment);
                if (m_used + padding + view.size > Capacity)
                {
                    return false;
                }
                payloadOffset = wrappedOffset;
                padding += wrappedOffset;
            }
        }
        else
        {
            const std::size_t alignedHead = alignUp(head, safeAlignment);
            if (alignedHead + view.size > tail)
            {
                return false;
            }
            payloadOffset = alignedHead;
            padding = alignedHead - head;
        }

        const std::size_t reservedBytes = padding + view.size;
        if (m_used + reservedBytes > Capacity)
        {
            return false;
        }

        std::memcpy(m_storage.data() + payloadOffset, view.data, view.size);

        m_head = (payloadOffset + view.size) % Capacity;
        m_used += reservedBytes;

        reservation.payloadOffset = payloadOffset;
        reservation.payloadSize = view.size;
        reservation.reservedBytes = reservedBytes;
        return true;
    }

    EventBufferView viewAt(const std::size_t offset, const std::size_t size) const
    {
        return EventBufferView{m_storage.data() + offset, size};
    }

    void release(const std::size_t reservedBytes)
    {
        if (reservedBytes == 0 || reservedBytes > m_used)
        {
            return;
        }

        m_tail = (m_tail + reservedBytes) % Capacity;
        m_used -= reservedBytes;
    }

    void reset()
    {
        m_head = 0;
        m_tail = 0;
        m_used = 0;
    }

private:
    static_assert(Capacity > 1, "EventsRingBuffer Capacity must be greater than 1");

    static std::size_t normalizeAlignment(const std::size_t alignment)
    {
        return alignment == 0 ? 1 : alignment;
    }

    static std::size_t alignUp(const std::size_t value, const std::size_t alignment)
    {
        const std::size_t rem = value % alignment;
        return rem == 0 ? value : value + (alignment - rem);
    }

    std::array<std::byte, Capacity> m_storage{};
    std::size_t m_head{0};
    std::size_t m_tail{0};
    std::size_t m_used{0};
};

} // namespace dynamic
} // namespace event_system
