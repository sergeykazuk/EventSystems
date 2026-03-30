#pragma once
#include "core/SystemTypes.hpp"
#include <memory>
#include <functional>


namespace event_system {

class LastEventDataHandler final
{
public:
    LastEventDataHandler();
    ~LastEventDataHandler();
    LastEventDataHandler(const LastEventDataHandler&) = delete;
    LastEventDataHandler(LastEventDataHandler&&) = delete;
    LastEventDataHandler& operator= (const LastEventDataHandler&) = delete;
    LastEventDataHandler& operator= (LastEventDataHandler&&) noexcept = delete;

    void updateLastEventData(const EventTypeEnum ev, BytePtr_t data);
    bool getLastEventData(const EventTypeEnum eventId,
        const std::function<void(std::byte const * const)>& visitor) const;

private:
    struct ClassData;
    std::unique_ptr<ClassData> m_pimpl;
};

}