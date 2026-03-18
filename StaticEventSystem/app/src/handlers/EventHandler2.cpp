#include "app/handlers/EventHandler2.hpp"
#include "system/EventSystemHelpers.hpp"
#include "system/EventSenders.hpp"
#include "types/ResultOfSum16.hpp"
#include "types/BoolFlags.hpp"
#include <iostream>

namespace app
{

EventHandler2::EventHandler2()
{
    event_system::registerEventHandler(getEventHandlerId(), this);
}

EventHandler2::~EventHandler2()
{
    event_system::unregisterEventHandler(getEventHandlerId());
}

void EventHandler2::handleEventTimestamp(const event_system::Timestamp& event)
{
    m_timestamp = event;
    std::cout << "[EventHandler2] Timestamp seconds: "
              << static_cast<int>(event.second) << "\n";
}

void EventHandler2::handleEventSumOfTwoInts(const event_system::ResultOfSum16& event)
{
    int16_t sum = m_timestamp.hour + m_timestamp.minute;
    if (event.valid)
    {
        sum += m_timestamp.second;
    }

    std::cout << "[EventHandler2] Sum result=" << event.result
              << ", calculated=" << sum
              << ", valid=" << std::boolalpha << event.valid << "\n";

    using Flags_t = event_system::BoolFlags;

    auto flag = (sum == event.result) ? Flags_t::eTrue : Flags_t::eFalse ;
    
    event_system::sendEvent(flag);
}

}
