#include "core/EventSystemTimer.hpp"
#include <algorithm>

namespace event_system {

    EventSystemTimer::EventSystemTimer(const ExpiredTimersCb_t& cb)
        : m_expiredTimersCb(cb)
    {}

    EventSystemTimer::~EventSystemTimer()
    {
        shutdown();
    }

    void EventSystemTimer::init()
    {
        bool expected = false;
        if (m_running.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        {
            m_thread = std::thread([this](){loop();});
        }
    }

    void EventSystemTimer::shutdown()
    {
        auto wasRunning = m_running.exchange(false, std::memory_order_acq_rel);
        if (wasRunning)
        {
            m_cv.notify_all();
            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }
    }

    void EventSystemTimer::startTimer(const EventTypeEnum event, std::chrono::milliseconds duration)
    {
        auto timerDue = Clock_t::now() + duration;
        {
            std::unique_lock lock(m_mutex);
            m_timersQueue.push_back({timerDue, event});
            std::sort(m_timersQueue.begin(), m_timersQueue.end(),
                [](const TimersData& l, const TimersData& r){
                    return l.m_timerDue < r.m_timerDue;
                });
        }
        m_cv.notify_one();
    }

    void EventSystemTimer::stopTimer(const EventTypeEnum event)
    {
        {
            std::unique_lock lock(m_mutex);
            m_timersQueue.erase(
                std::remove_if(m_timersQueue.begin(), m_timersQueue.end(),
                               [event](const TimersData& t) { return t.m_timerEvent == event; }),
                m_timersQueue.end());
        }
        m_cv.notify_one();
    }


    void EventSystemTimer::loop()
    {
        std::vector<EventTypeEnum> expiredTimers{};
        while (m_running)
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                if (m_timersQueue.empty())
                {
                    m_cv.wait(lock, [this]()
                              { return !m_timersQueue.empty() || !m_running; });
                    continue;
                }

                const auto due = m_timersQueue.front().m_timerDue;

                m_cv.wait_until(lock, due, [this, due]()
                                {
                                    return !m_running || m_timersQueue.empty() || m_timersQueue.front().m_timerDue != due;
                                });
                if (!m_running)
                {
                    break;
                }

                if (m_timersQueue.empty())
                {
                    continue;
                }

                // iterate over all timers to find expired ones
                const auto nowPoint = Clock_t::now();

                while (!m_timersQueue.empty() && m_timersQueue.front().m_timerDue <= nowPoint)
                {
                    expiredTimers.push_back(m_timersQueue.front().m_timerEvent);
                    m_timersQueue.erase(m_timersQueue.begin());
                }
            }

            if (!expiredTimers.empty())
            {
                m_expiredTimersCb(std::move(expiredTimers));
                expiredTimers.clear();
            }
        }
    }

}
