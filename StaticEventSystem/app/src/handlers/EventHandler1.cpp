#include "app/handlers/EventHandler1.hpp"
#include "system/EventSystemHelpers.hpp"
#include "types/Timestamp.hpp"
#include "types/ResultOfSum16.hpp"
#include "system/EventSenders.hpp"
#include <iostream>

namespace app
{

EventHandler1::EventHandler1()
{
    event_system::registerEventHandler(getEventHandlerId(), this);
}

EventHandler1::~EventHandler1()
{
    event_system::unregisterEventHandler(getEventHandlerId());
}

void EventHandler1::handleEventEventSystemReady()
{
    std::cout << "[EventHandler1] EventSystemReady\n";
}

void EventHandler1::handleEventEventSystemShutdown()
{
    std::cout << "[EventHandler1] EventSystemShutdown\n";
}

void EventHandler1::handleEventTimestamp(const event_system::Timestamp& eventPayload)
{
    std::cout << "[EventHandler1] Timestamp: "
              << static_cast<int>(eventPayload.hour) << ":"
              << static_cast<int>(eventPayload.minute) << ":"
              << static_cast<int>(eventPayload.second) << "\n";

    static bool switcher = false;
    event_system::ResultOfSum16 tSum{};
    tSum.result = static_cast<int16_t>(eventPayload.hour + eventPayload.minute);

    tSum.result += switcher ? (eventPayload.second) : 0;
    tSum.valid = switcher;
    
    event_system::sendEvent(tSum);

    switcher = !switcher;
}

}
