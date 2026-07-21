#pragma once
#include "HandlersManager.hpp"
#include "EventQueue.hpp"
#include "TypeEraser.hpp"
#include <type_traits>
#include <utility>

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
        static_assert(isRegisteredPayloadType<StoredT>(),
            "DynamicEventSystem payloads must be trivially copyable");

        auto id = TypeID::value<StoredT>();

        // `stored` is copied into the task's own closure storage here, so its
        // lifetime is tied to the queued task, not to the caller's argument.
        // `&m_handlersManager` is captured by reference: it's a member of this
        // singleton, which outlives every task ever queued (tasks are drained
        // in EventQueue::stop()/dtor before the singleton itself is torn down).
        m_eventQueue.addTask([&handlers = m_handlersManager, id, stored = StoredT(std::forward<T>(ev))]()
        {
            const EventBufferView view{
                reinterpret_cast<const std::byte*>(&stored),
                sizeof(StoredT)
            };
            handlers.dispatchEvent(id, view);
        });
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
        , m_eventQueue()
    {
    }

private:
    HandlersManager m_handlersManager;
    EventQueue m_eventQueue;
};

}
}