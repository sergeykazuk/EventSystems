#pragma once

#include "handlers/IEventHandler_2.hpp"
#include "types/Timestamp.hpp"

namespace app
{

class EventHandler2 final : protected event_system::IEventHandler_2
{
public:
    EventHandler2();
    ~EventHandler2();

    EventHandler2(const EventHandler2&) = delete;
    EventHandler2& operator=(const EventHandler2&) = delete;
    EventHandler2(EventHandler2&&) = delete;
    EventHandler2& operator=(EventHandler2&&) = delete;

private:
    void handleEventTimestamp(const event_system::Timestamp& event) override;
    void handleEventSumOfTwoInts(const event_system::ResultOfSum16& event) override;

private:
    event_system::Timestamp m_timestamp{};
};

}
