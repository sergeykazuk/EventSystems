#pragma once
#include <memory>
#include "core/SystemTypes.hpp"

namespace event_system {

class EventSystem
{
public:

    static EventSystem& getInstance();

    void sendEvent(const EventTypeEnum, BytePtr_t&&);

    void registerEventHandler(const EventHandlerId, IEventHandler* const);

    void unregisterEventHandler(const EventHandlerId);

    const BytePtr_t& getLastEventData(const EventTypeEnum) const;

    void init();
    void shutdown();

    ~EventSystem() = default;
    EventSystem(const EventSystem&) = delete;
    EventSystem(EventSystem&&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;
    EventSystem& operator=(EventSystem&&) = delete;

private:
    EventSystem();

private:
    struct ClassData;
    std::unique_ptr<ClassData> m_pimpl;
};

}