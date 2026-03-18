#pragma once
#include <cstdint>

namespace event_system {

class IEventHandler;
enum class EventHandlerId : uint16_t;

void registerEventHandler(const EventHandlerId, IEventHandler* const);
void unregisterEventHandler(const EventHandlerId);

}