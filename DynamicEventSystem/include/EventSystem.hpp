#pragma once
#include "HandlersManager.hpp"
#include "EventQueue.hpp"
#include "TypeEraser.hpp"

namespace event_system {
namespace dynamic {

class EventSystem
{
public:

    static EventSystem& getInstance();

    template <typename T>
    void sendEvent(T&& ev)
    {
        using StoredT = std::decay_t<T>;
        auto id = TypeID::value<StoredT>();

        auto bytePtr = createEvent(std::forward<T>(ev));
        m_eventQueue.addEvent(id, std::move(bytePtr));
    }

    template <typename T>
    void registerCallback(std::function<void(const T&)> cb)
    {
        m_handlersManager.registerCallback(std::move(cb));
    }

    template <typename T>
    void unregisterCallback(std::function<void(const T&)>)
    {
    }

    void init();
    void shutdown();

    ~EventSystem() = default;
    EventSystem(const EventSystem&) = delete;
    EventSystem(EventSystem&&) = default;
    EventSystem& operator=(const EventSystem&) = delete;
    EventSystem& operator=(EventSystem&&) = default;

private:
    EventSystem()
        : m_handlersManager()
        , m_eventQueue([this](TypeID_t id, const BytePtr& data)
            { m_handlersManager.dispatchEvent(id, data); })
    {
    }

private:
    HandlersManager m_handlersManager;
    EventQueue m_eventQueue;
};

}
}