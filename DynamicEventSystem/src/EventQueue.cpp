#include "EventQueue.hpp"
#include <queue>
#include <unordered_map>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <atomic>

namespace event_system::dynamic {

struct EventQueue::ClassData
{
    ~ClassData();

    void dispatcher();

    using EventPair_t = std::pair<TypeID_t, BytePtr>;

    std::function<void(TypeID_t, const BytePtr&)> m_handler;
    std::queue<EventPair_t> m_eventQueue;

    std::unordered_map<TypeID_t, BytePtr> m_lastEvents;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_running{false};
    std::thread m_runner;
};

EventQueue::EventQueue(std::function<void(TypeID_t, const BytePtr&)> handler)
    : m_pimpl(std::make_unique<ClassData>())
{
    m_pimpl->m_handler = std::move(handler);
}

EventQueue::~EventQueue()
{
    stop();
}

void EventQueue::addEvent(TypeID_t id, BytePtr data)
{
    if (data == nullptr)
    {
        return;
    } 

    {
        std::lock_guard<std::mutex> lock(m_pimpl->m_mutex);
        m_pimpl->m_eventQueue.push(std::make_pair(id, std::move(data)));
    }
    m_pimpl->m_cv.notify_one();
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

    if (m_pimpl->m_runner.joinable() && m_pimpl->m_runner.get_id() != std::this_thread::get_id())
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

        m_handler(tPair.first, tPair.second);

        m_lastEvents.insert_or_assign(tPair.first, std::move(tPair.second));
    }
}

}