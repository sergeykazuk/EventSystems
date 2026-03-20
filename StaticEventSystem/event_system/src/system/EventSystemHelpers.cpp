#include "system/EventSystemHelpers.hpp"
#include "core/EventSystem.hpp"

namespace event_system {

EventSystemOperationResult registerEventHandler(const EventHandlerId hId, IEventHandler* const ptr)
{
    return EventSystem::getInstance().registerEventHandler(hId, ptr);
}

EventSystemOperationResult unregisterEventHandler(const EventHandlerId hId)
{
    return EventSystem::getInstance().unregisterEventHandler(hId);
}

}