#include "EventSystem.hpp"
#include "events.hpp"
#include <thread>
#include <csignal>
#include <array>
#include <iostream>
#include <chrono>
#include <atomic>

namespace {

using namespace event_system::dynamic;

std::array<std::thread, 2> g_senders;
std::atomic<bool> g_interrupted(false);

void signalHandler(int signal) {
    if (signal == SIGINT) {
        g_interrupted = true;
    }
}

void sendTimestamp()
{
    using namespace std::chrono;

    uint32_t counter = 0;
    while (!g_interrupted)
    {
        auto timestamp = std::chrono::system_clock::now();
        
        EventSystem::getInstance().sendEvent(events::Timestamp{counter++, system_clock::now()});
        std::this_thread::sleep_for(150ms);    
    }
}

void sendSpeed()
{
    using namespace std::chrono;
    constexpr uint8_t N = 5;
    constexpr std::array<float, N> speeds{10.1f, 20.f, 40.5f, 21.f, 13.9f};
    uint8_t index = 0;
    auto unit = events::Speed::Unit::eKmh;

    while (!g_interrupted)
    {
        EventSystem::getInstance().sendEvent(events::Speed{speeds[index], unit});
        
        index = (index + 1 == N) ? 0 : index + 1;
        
        std::this_thread::sleep_for(100ms);    
    }
}

void onSpeed(const events::Speed& spd)
{
    std::cout << "speed " << spd.speed << "; unit " 
        << static_cast<uint8_t>(spd.unit) << std::endl;
}

void onTimestamp(const events::Timestamp& stmp)
{
    using namespace std::chrono;
    using ES = event_system::dynamic::EventSystem;

    if (stmp.fromCb)
    {
        return;
    }

    auto now = system_clock::now();
    auto diff = now - stmp.timestamp;
    auto dispatchTime = duration_cast<microseconds>(diff);

    std::cout << "timestamp " << stmp.timestamp.time_since_epoch().count() 
        << "; counter " << stmp.counter << "; from CB " << stmp.fromCb 
        << "; dispatch time " << dispatchTime.count() << "us" << std::endl;

    ES::getInstance().sendEvent(events::Timestamp{stmp.counter, now, true});
}

void onTimestamp2(const events::Timestamp& stmp)
{
    if (!stmp.fromCb)
    return;

    std::cout << "timestamp " << stmp.timestamp.time_since_epoch().count() 
        << "; counter " << stmp.counter << "; from CB " << stmp.fromCb << std::endl;
}

}



int main()
{
    std::signal(SIGINT, signalHandler);
    using ES =event_system::dynamic::EventSystem;
    using namespace event_system::dynamic::events;

    ES::getInstance().registerCallback<Timestamp>([](const Timestamp& stmp)
        {
            onTimestamp(stmp);
        });
    ES::getInstance().registerCallback<Timestamp>([](const Timestamp& stmp)
        {
            onTimestamp2(stmp);
        });
        
    ES::getInstance().registerCallback<Speed>([](const Speed& spd)
        {
            onSpeed(spd);
        });


    event_system::dynamic::EventSystem::getInstance().init();

    g_senders[0] = std::thread(sendTimestamp);
    g_senders[1] = std::thread(sendSpeed);

    // Wait until Ctrl+C is pressed
    while (!g_interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for (auto& thr: g_senders)
    {
        thr.join();
    }

    ES::getInstance().shutdown();

}