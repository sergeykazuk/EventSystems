#include "core/EventDispatcher.hpp"
#include "core/IEventHandler.hpp"

namespace event_system {

EventDispatcher::EventDispatcher(EventHandlersMap_t&& eventHandlersMap)
    : m_eventHandlersMap(std::forward<EventHandlersMap_t>(eventHandlersMap))
{
}

void EventDispatcher::setEventHandlerPtr(const EventHandlerId id, IEventHandler* ptr)
{
    m_handlersMap.insert_or_assign(id, ptr);
}

void EventDispatcher::dispatchEvent(const EventTypeEnum& eventId, const BytePtr_t& data) const
{
    const auto& handlerIds = m_eventHandlersMap.at(eventId);

    for (auto& handlerId: handlerIds)
    {
        auto ptr = m_handlersMap.at(handlerId);
        if (ptr != nullptr)
        {
            ptr->processEvent(eventId, data);
        }
    }
}

}