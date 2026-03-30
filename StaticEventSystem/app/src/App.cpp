#include "app/App.hpp"
#include "core/EventSystem.hpp"
#include "system/EventSenders.hpp"
#include "types/Timestamp.hpp"
#include <csignal>
#include <chrono>
#include <ctime>
#include <thread>
#include <iostream>
#include <termios.h>
#include <unistd.h>

namespace {
    std::atomic<bool> gStopRequested{false};

    void sigHandler(const int signal)
    {
        if (signal == SIGINT)
        {
            gStopRequested.store(true, std::memory_order_relaxed);
        }
    }

    void keyboardListener()
    {
        // Save old terminal settings
        termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        while (!gStopRequested.load(std::memory_order_relaxed)) {
            int ch = getchar();
            if (ch == 27) { // Escape key
                gStopRequested.store(true, std::memory_order_relaxed);
                break;
            }
        }

        // Restore old terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}


    namespace app {

int App::run()
{
    using namespace std::chrono;
    std::signal(SIGINT, sigHandler);

    std::thread inputThread(keyboardListener);

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

    inputThread.join();
    std::cout << "Test App will be stopped" << std::endl;

    event_system::EventSystem::getInstance().shutdown();
    return 0;
}
} // namespace app
