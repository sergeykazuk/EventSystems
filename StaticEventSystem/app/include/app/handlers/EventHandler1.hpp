#pragma once

#include "handlers/IEventHandler_1.hpp"

namespace app
{

class EventHandler1 final : protected event_system::IEventHandler_1
{
public:
    EventHandler1();
    ~EventHandler1();

    EventHandler1(const EventHandler1&) = delete;
    EventHandler1& operator=(const EventHandler1&) = delete;
    EventHandler1(EventHandler1&&) = delete;
    EventHandler1& operator=(EventHandler1&&) = delete;

private:
    void handleEventEventSystemReady() override;
    void handleEventEventSystemShutdown() override;
    void handleEventTimestamp(const event_system::Timestamp& eventPayload) override;
};

}
