#include "system/EventSystemHelpers.hpp"
#include "system/EventSystem.hpp"

namespace event_system {

void registerEventHandler(const EventHandlerId hId, IEventHandler* const ptr)
{
    EventSystem::getInstance().registerEventHandler(hId, ptr);
}

void unregisterEventHandler(const EventHandlerId hId)
{
    EventSystem::getInstance().unregisterEventHandler(hId);
}

}