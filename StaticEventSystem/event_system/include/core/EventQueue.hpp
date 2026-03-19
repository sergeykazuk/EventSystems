#pragma once
#include <memory>
#include <functional>
#include "core/SystemTypes.hpp"

namespace event_system {

class EventQueue
{
public:
    explicit EventQueue(const EventDispatcher&);
    ~EventQueue();
    EventQueue(const EventQueue&) = delete;
    EventQueue& operator=(const EventQueue&) = delete;
    EventQueue(EventQueue&&) = delete;
    EventQueue& operator=(EventQueue&&) = delete;

    void addEvent(const EventTypeEnum, BytePtr_t&& data);
    bool getLastEventData(const EventTypeEnum,
        const std::function<void(const BytePtr_t&)>& visitor) const;
    void start();
    void stop();

private:

    struct ClassData;
    std::unique_ptr<ClassData> m_pimpl;
};

}