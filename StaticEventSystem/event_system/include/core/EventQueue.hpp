#pragma once
#include <memory>
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
    const BytePtr_t& getLastEventData(const EventTypeEnum) const;
    void start();
    void stop();

private:

    struct ClassData;
    std::unique_ptr<ClassData> m_pimpl;
};

}