#include "app/App.hpp"
#include "system/EventSystem.hpp"
#include "system/EventSenders.hpp"
#include "types/Timestamp.hpp"
#include <csignal>
#include <chrono>
#include <ctime>
#include <thread>

namespace {
    std::atomic<bool> gStopRequested{false};

    void sigHandler(const int signal)
    {
        if (signal == SIGINT)
        {
            gStopRequested.store(true, std::memory_order_relaxed);
        }
    }
}

namespace app
{

int App::run()
{
    using namespace std::chrono;
    std::signal(SIGINT, sigHandler);

    event_system::EventSystem::getInstance().init();

    event_system::Timestamp tTimestamp{};
    tTimestamp.format = event_system::TimeFormat::e24h;

    while (!gStopRequested.load(std::memory_order_relaxed))
    {
        std::this_thread::sleep_for(milliseconds(500));

        const auto now = system_clock::now();
        const auto nowAsTimeT = system_clock::to_time_t(now);

        std::tm localTime{};
        localtime_r(&nowAsTimeT, &localTime);

        auto second = static_cast<uint8_t>(localTime.tm_sec);
        if (second != tTimestamp.second)
        {
            tTimestamp.hour = static_cast<uint8_t>(localTime.tm_hour);
            tTimestamp.minute = static_cast<uint8_t>(localTime.tm_min);
            tTimestamp.second = static_cast<uint8_t>(localTime.tm_sec);

            event_system::sendEvent(tTimestamp);
        }
    }

    event_system::EventSystem::getInstance().shutdown();
    return 0;
}

}
