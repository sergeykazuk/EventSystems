#pragma once
#include "typedefs.hpp"
#include <functional>

namespace event_system {
namespace dynamic {

class EventQueue
{
public:

    EventQueue(std::function<void(TypeID_t, const BytePtr&)> handler);
    EventQueue() = delete;
    ~EventQueue();
    EventQueue(const EventQueue&) = delete;
    EventQueue(EventQueue&&) = default;
    EventQueue& operator=(const EventQueue&) = delete;
    EventQueue& operator=(EventQueue&&) noexcept = default;

    void addEvent(TypeID_t id, BytePtr data);
    void start();
    void stop();

private:

    struct ClassData;
    std::unique_ptr<ClassData> m_pimpl;
};

}
}