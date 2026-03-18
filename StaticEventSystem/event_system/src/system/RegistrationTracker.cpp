#include "core/RegistrationTracker.hpp"
#include <chrono>
#include <iostream>
#include <vector>

namespace event_system
{

RegistrationTracker::RegistrationTracker()
    : m_watchdogThread([this]() {
        
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(5));

            std::vector<EventHandlerId> unregistered;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_registeredHandlers.all() 
                    || m_stopWatchdog.load(std::memory_order_relaxed))
                {
                    return;
                }

                for (size_t index = 0; index < m_registeredHandlers.size(); ++index)
                {
                    if (!m_registeredHandlers.test(index))
                    {
                        unregistered.push_back(static_cast<EventHandlerId>(index));
                    }
                }
            }

            std::cout << "Unregistered event handlers:\n";
            for (const auto handlerId : unregistered)
            {
                std::cout << "- " << toString(handlerId) << "\n";
            }
        }
    })
{
}

RegistrationTracker::~RegistrationTracker()
{
    m_stopWatchdog.store(true, std::memory_order_relaxed);
    if (m_watchdogThread.joinable())
    {
        m_watchdogThread.join();
    }
}

bool RegistrationTracker::registerHandler(const EventHandlerId handlerId)
{
    const auto index = static_cast<size_t>(handlerId);
    if (index >= m_registeredHandlers.size())
    {
        std::cout << toString(handlerId) << " is out of bound!\n";        
        return false;
    }

    if (m_registeredHandlers.test(index))
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_registeredHandlers.set(index);
    }
    return true;
}

void RegistrationTracker::unregisterHandler(const EventHandlerId handlerId)
{
    const auto index = static_cast<size_t>(handlerId);
    if (index >= m_registeredHandlers.size())
    {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_registeredHandlers.reset(index);
}

bool RegistrationTracker::allRegistered() const
{
    return m_registeredHandlers.all();
}

}