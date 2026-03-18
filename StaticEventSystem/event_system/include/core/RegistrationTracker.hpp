#pragma once

#include "core/EventHandlerId.hpp"
#include <atomic>
#include <bitset>
#include <mutex>
#include <thread>

namespace event_system
{

class RegistrationTracker
{
public:
    RegistrationTracker();
    ~RegistrationTracker();

    bool registerHandler(const EventHandlerId handlerId);
    void unregisterHandler(const EventHandlerId handlerId);
    bool allRegistered() const;

private:
    mutable std::mutex m_mutex;
    std::bitset<static_cast<size_t>(EventHandlerId::eHandlersCount)> m_registeredHandlers{};
    std::atomic<bool> m_stopWatchdog{false};
    std::thread m_watchdogThread;
};

}