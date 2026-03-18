#include "core/EventQueue.hpp"
#include "core/EventDispatcher.hpp"
#include <queue>
#include <unordered_map>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <atomic>

namespace event_system {

struct EventQueue::ClassData
{
    ClassData(const EventDispatcher& dispatcher)
        : m_eventDispatcher(dispatcher)
    {}
    ~ClassData();

    void dispatcher();

    const EventDispatcher& m_eventDispatcher;

    using EventPair_t = std::pair<EventTypeEnum, BytePtr_t>;

    std::queue<EventPair_t> m_eventQueue;

    std::unordered_map<EventTypeEnum, BytePtr_t> m_lastEvents;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_running{false};
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
    if (!m_pimpl->m_running)
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_pimpl->m_mutex);
        m_pimpl->m_eventQueue.push(std::make_pair(id, std::move(data)));
    }
    m_pimpl->m_cv.notify_one();
}

const BytePtr_t& EventQueue::getLastEventData(const EventTypeEnum eventId) const
{
    static const BytePtr_t emptyData{};

    const auto it = m_pimpl->m_lastEvents.find(eventId);
    if (it == m_pimpl->m_lastEvents.end())
    {
        return emptyData;
    }

    return it->second;
}

void EventQueue::start()
{
    bool expected = false;
    if (m_pimpl->m_running.compare_exchange_strong(expected, true))
    {
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
    bool wasRunning = m_pimpl->m_running.exchange(false);
    if (wasRunning)
    {
        m_pimpl->m_cv.notify_one();
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
    while (m_running)
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

        m_lastEvents.insert_or_assign(tPair.first, std::move(tPair.second));
    }
}

}