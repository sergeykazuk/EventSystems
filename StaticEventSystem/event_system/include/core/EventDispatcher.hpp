#pragma once
#include <unordered_map>
#include "core/SystemTypes.hpp"

namespace event_system {

class EventDispatcher
{
public:
    EventDispatcher(EventHandlersMap_t&& eventHandlersMap);
    void setEventHandlerPtr(const EventHandlerId, IEventHandler*);

    void dispatchEvent(const EventTypeEnum&, const BytePtr_t&) const;

private:
    const EventHandlersMap_t m_eventHandlersMap;
    std::unordered_map<EventHandlerId, IEventHandler*> m_handlersMap;
};

}