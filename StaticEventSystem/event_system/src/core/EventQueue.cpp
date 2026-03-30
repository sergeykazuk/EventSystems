#include "core/EventQueue.hpp"
#include "core/EventDispatcher.hpp"
#include "core/EventPayloadHelpers.hpp"
#include "core/LastEventDataHandler.hpp"
#include <queue>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <atomic>

namespace {

using EventPair_t = std::pair<event_system::EventTypeEnum, event_system::BytePtr_t>;
using EventQueue_t = std::queue<EventPair_t>;

}

namespace event_system {

struct EventQueue::ClassData
{
    ClassData(const EventDispatcher& dispatcher)
        : m_eventDispatcher(dispatcher)
    {}
    ~ClassData();

    void dispatcher();

    const EventDispatcher& m_eventDispatcher;
    EventQueue_t m_eventQueue{};
    LastEventDataHandler m_lastEventDataHandler{};
    std::mutex m_mutex{};
    std::condition_variable m_cv{};
    bool m_running{false};
    std::thread m_runner;
};

EventQueue::EventQueue(const EventDispatcher& dispatcher)
    : m_pimpl(std::make_unique<ClassData>(dispatcher))
{
}

EventQueue::~EventQueue()
{
    stop();
}

void EventQueue::addEvent(const EventTypeEnum id, BytePtr_t&& data)
{
    {
        std::lock_guard<std::mutex> lock(m_pimpl->m_mutex);

        // Re-check under lock to avoid enqueue after stop() swap/clear.
        if (!m_pimpl->m_running)
        {
            return;
        }

        m_pimpl->m_eventQueue.push(std::make_pair(id, std::move(data)));
    }
    m_pimpl->m_cv.notify_one();
}

bool EventQueue::getLastEventData(const EventTypeEnum eventId,
    const std::function<void(std::byte const * const)>& visitor) const
{
    return m_pimpl->m_lastEventDataHandler.getLastEventData(eventId, visitor);
}

void EventQueue::start()
{
    std::lock_guard<std::mutex> lock(m_pimpl->m_mutex);

    if (!m_pimpl->m_running)
    {
        m_pimpl->m_running = true;

        if (m_pimpl->m_runner.joinable())
        {
            m_pimpl->m_runner.join();
        }
        m_pimpl->m_runner = std::thread([this](){ m_pimpl->dispatcher(); });
    }
    else
    {
        std::cout << "Thread is already started\n";
    }
}

void EventQueue::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_pimpl->m_mutex);
        if (m_pimpl->m_running)
        {
            m_pimpl->m_running = false;
            m_pimpl->m_cv.notify_all();
        }

        EventQueue_t tEmptyQueue{};
        m_pimpl->m_eventQueue.swap(tEmptyQueue);
    }

    if (m_pimpl->m_runner.joinable() 
        && m_pimpl->m_runner.get_id() != std::this_thread::get_id())
    {
        m_pimpl->m_runner.join();
    }
}

EventQueue::ClassData::~ClassData()
{
    if (m_runner.joinable())
    {
        m_runner.join();
    }
}

void EventQueue::ClassData::dispatcher()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this]() {
            return !m_eventQueue.empty() || !m_running;
        });

        if (!m_running)
        {
            lock.unlock();
            break;
        }

        auto tPair = std::move(m_eventQueue.front());
        m_eventQueue.pop();

        lock.unlock();

        m_eventDispatcher.dispatchEvent(tPair.first, tPair.second);
        m_lastEventDataHandler.updateLastEventData(tPair.first, std::move(tPair.second));
    }
}

}