#pragma once
#include "core/SystemTypes.hpp"

#include "core/EventTypeEnum.hpp"
#include "core/EventHandlerId.hpp"
#include "core/EventQueueId.hpp"

namespace event_system {

EventHandlersMap_t buildEventHandlersMap(const EventQueueId);

}
