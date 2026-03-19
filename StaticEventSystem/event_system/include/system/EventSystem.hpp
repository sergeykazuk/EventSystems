#pragma once
#include <memory>
#include "core/SystemTypes.hpp"
#include "core/EventSystemOperationResult.hpp"

namespace event_system {

class EventSystem
{
public:

    static EventSystem& getInstance();

    void sendEvent(const EventTypeEnum, BytePtr_t&&);

    [[nodiscard]] EventSystemOperationResult registerEventHandler(const EventHandlerId, IEventHandler* const);

    [[nodiscard]] EventSystemOperationResult unregisterEventHandler(const EventHandlerId);

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