#pragma once
#include <chrono>
#include <thread>
#include <cstdint>
#include <functional>
#include <mutex>
#include <atomic>
#include <vector>
#include <condition_variable>

namespace event_system {

enum class EventTypeEnum : uint16_t;

class EventSystemTimer final
{
using Clock_t = std::chrono::steady_clock;

struct TimersData
{
    std::chrono::time_point<Clock_t> m_timerDue{};
    EventTypeEnum m_timerEvent{};
};

public:
    using ExpiredTimersCb_t = std::function<void(std::vector<EventTypeEnum>)>;

    EventSystemTimer(const ExpiredTimersCb_t& cb);
    ~EventSystemTimer();
    EventSystemTimer(const EventSystemTimer&) = delete;
    EventSystemTimer(EventSystemTimer&&) = delete;
    EventSystemTimer& operator=(const EventSystemTimer&) = delete;
    EventSystemTimer& operator=(EventSystemTimer&&) = delete;

    void init();
    void shutdown();

    void startTimer(const EventTypeEnum event, std::chrono::milliseconds duration);
    void stopTimer(const EventTypeEnum event);

private:
    void loop();

private:
    std::vector<TimersData> m_timersQueue{};
    ExpiredTimersCb_t m_expiredTimersCb{};
    std::mutex m_mutex{};
    std::condition_variable m_cv{};
    std::thread m_thread{};
    std::atomic<bool> m_running{false};
};

}