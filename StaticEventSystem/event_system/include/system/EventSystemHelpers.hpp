#pragma once
#include "core/EventSystemOperationResult.hpp"

namespace event_system {

class IEventHandler;
enum class EventHandlerId : uint16_t;

[[nodiscard]] EventSystemOperationResult registerEventHandler(const EventHandlerId, IEventHandler* const);
[[nodiscard]] EventSystemOperationResult unregisterEventHandler(const EventHandlerId);

}