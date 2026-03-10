#include "EventSystem.hpp"

namespace event_system::dynamic 
{
    EventSystem& EventSystem::getInstance()
    {
        static EventSystem sEv{};
        return sEv;
    }

    void EventSystem::init()
    {
        m_eventQueue.start();
    }

    void EventSystem::shutdown()
    {
        m_eventQueue.stop();
    }
}