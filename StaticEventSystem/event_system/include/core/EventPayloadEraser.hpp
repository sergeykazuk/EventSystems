#pragma once
#include "core/SystemTypes.hpp"
#include <cstring>
#include <new>
#include <type_traits>
#include <utility>

namespace event_system {

template <typename T>
BytePtr_t erasePayload(T&& eventPayload)
{
    using EventPayloadT = std::decay_t<T>;

    auto erased = std::make_unique<std::byte[]>(sizeof(EventPayloadT));

    if constexpr (std::is_trivially_copyable_v<EventPayloadT>)
    {
        std::memcpy(erased.get(), &eventPayload, sizeof(EventPayloadT));
        return erased;
    }

    new (erased.get()) EventPayloadT(std::forward<T>(eventPayload));
    return erased;
}

}