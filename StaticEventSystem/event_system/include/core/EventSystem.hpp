#pragma once
#include <memory>
#include <functional>
#include <chrono>
#include "core/SystemTypes.hpp"
#include "core/EventSystemOperationResult.hpp"

namespace event_system {

class EventSystem
{
public:

    static EventSystem& getInstance();

    void sendEvent(const EventTypeEnum, BytePtr_t&&);
    void sendTimedEvent(const EventTypeEnum, std::chrono::milliseconds);
    void stopTimedEvent(const EventTypeEnum);

    [[nodiscard]] EventSystemOperationResult registerEventHandler(const EventHandlerId, IEventHandler* const);

    [[nodiscard]] EventSystemOperationResult unregisterEventHandler(const EventHandlerId);

    bool getLastEventData(const EventTypeEnum,
        const std::function<void(std::byte const * const)>& visitor) const;

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