#include "core/EventQueue.hpp"
#include "core/EventDispatcher.hpp"
#include "core/EventPayloadHelpers.hpp"
#include <queue>
#include <unordered_map>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <atomic>

namespace {

using EventPair_t = std::pair<event_system::EventTypeEnum, event_system::BytePtr_t>;
using EventQueue_t = std::queue<EventPair_t>;

void destroyQueuedPayloads(EventQueue_t& queue)
{
    while (!queue.empty())
    {
        auto& event = queue.front();
        destroyEventPayload(event.first, event.second);
        queue.pop();
    }
}

void destroyLastEvents(
    std::unordered_map<event_system::EventTypeEnum, event_system::BytePtr_t>& lastEvents)
{
    for (auto& [eventId, payload] : lastEvents)
    {
        destroyEventPayload(eventId, payload);
    }
    lastEvents.clear();
}

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
    EventQueue_t m_eventQueue;

    std::unordered_map<EventTypeEnum, BytePtr_t> m_lastEvents;
    std::mutex m_lastEventsMutex;
    std::mutex m_mutex;
    std::condition_variable m_cv;
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
    const std::function<void(const BytePtr_t&)>& visitor) const
{
    {
        std::lock_guard<std::mutex> lock(m_pimpl->m_lastEventsMutex);

        const auto it = m_pimpl->m_lastEvents.find(eventId);
        if (it == m_pimpl->m_lastEvents.end())
        {
            return false;
        }

        visitor(it->second);
    }
    return true;
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

        destroyQueuedPayloads(m_pimpl->m_eventQueue);
    }

    if (m_pimpl->m_runner.joinable() 
        && m_pimpl->m_runner.get_id() != std::this_thread::get_id())
    {
        m_pimpl->m_runner.join();
    }
    
    {
        std::lock_guard<std::mutex> lock(m_pimpl->m_lastEventsMutex);
        destroyLastEvents(m_pimpl->m_lastEvents);
    }
}

EventQueue::ClassData::~ClassData()
{
    if (m_runner.joinable())
    {
        m_runner.join();
    }
    
    if (!m_eventQueue.empty())
    {
        destroyQueuedPayloads(m_eventQueue);
    }

    if (!m_lastEvents.empty())
    {
        destroyLastEvents(m_lastEvents);
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

        {
            std::lock_guard<std::mutex> lastEventsLock(m_lastEventsMutex);
            const auto it = m_lastEvents.find(tPair.first);
            if (it != m_lastEvents.end())
            {
                destroyEventPayload(it->first, it->second);
            }

            m_lastEvents.insert_or_assign(tPair.first, std::move(tPair.second));
        }
    }
}

}