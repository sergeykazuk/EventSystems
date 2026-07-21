#include "EventQueue.hpp"
#include <queue>
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

    std::queue<std::function<void()>> m_tasks;

    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_running{false};
    std::thread m_runner;
};

EventQueue::EventQueue()
    : m_pimpl(std::make_unique<ClassData>())
{
}

EventQueue::~EventQueue()
{
    stop();
}

void EventQueue::addTask(std::function<void()> task)
{
    if (!task)
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_pimpl->m_mutex);

        if (!m_pimpl->m_running.load(std::memory_order_relaxed))
        {
            return;
        }

        m_pimpl->m_tasks.push(std::move(task));
    }
    m_pimpl->m_cv.notify_one();
}

void EventQueue::start()
{
    std::lock_guard<std::mutex> lock(m_pimpl->m_mutex);
    bool expected = false;
    if (m_pimpl->m_running.compare_exchange_strong(expected, true, std::memory_order_release))
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
    {
        std::lock_guard<std::mutex> lock(m_pimpl->m_mutex);
        bool wasRunning = m_pimpl->m_running.exchange(false, std::memory_order_release);
        if (!wasRunning)
        {
            return;
        }

        std::queue<std::function<void()>> emptyQueue{};
        m_pimpl->m_tasks.swap(emptyQueue);

        m_pimpl->m_cv.notify_all();
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
    while (true)
    {
        std::function<void()> task;

        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this]() {
            return !m_tasks.empty() || !m_running.load(std::memory_order_acquire);
        });

        if (!m_running.load(std::memory_order_acquire))
        {
            break;
        }

        task = std::move(m_tasks.front());
        m_tasks.pop();

        lock.unlock();

        task();
    }
}

}