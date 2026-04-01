#pragma once
#include "core/SystemTypes.hpp"
#include <cstring>
#include <new>
#include <utility>

namespace event_system {

template <typename T>
BytePtr_t erasePayload(T&& eventPayload)
{
    using EventPayloadT = std::decay_t<T>;

    static auto deleter = [](std::byte* ptr) {
        reinterpret_cast<EventPayloadT*>(ptr)->~EventPayloadT();
        delete[] ptr;
    };

    std::unique_ptr<std::byte[], decltype(deleter)> 
            erased{new std::byte[sizeof(EventPayloadT)], deleter};

    if constexpr (std::is_trivially_copyable_v<EventPayloadT>)
    {
        std::memcpy(erased.get(), &eventPayload, sizeof(EventPayloadT));
        return erased;
    }

    new (erased.get()) EventPayloadT(std::forward<T>(eventPayload));
    return erased;
}

}