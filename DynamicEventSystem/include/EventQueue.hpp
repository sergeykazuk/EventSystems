#pragma once
#include "typedefs.hpp"
#include <functional>
#include <memory>

namespace event_system {
namespace dynamic {

// Plain, generic FIFO task queue with a dedicated worker thread. It has no
// knowledge of events/payloads/handlers - callers hand it self-contained
// std::function<void()> tasks (typically lambdas that own their own copy of
// whatever data they need) and it runs them in order on the worker thread.
class EventQueue
{
public:

    EventQueue();
    ~EventQueue();
    EventQueue(const EventQueue&) = delete;
    EventQueue(EventQueue&&) = default;
    EventQueue& operator=(const EventQueue&) = delete;
    EventQueue& operator=(EventQueue&&) noexcept = default;

    void addTask(std::function<void()> task);

    void start();
    void stop();

private:

    struct ClassData;
    std::unique_ptr<ClassData> m_pimpl;
};

}
}