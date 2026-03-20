#include "core/EventSystemTimer.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>


namespace event_system {

enum class EventTypeEnum : uint16_t
{
    eTimerA = 1,
    eTimerB = 2,
    eTimerC = 3,
    eTimerD = 4,
};

namespace {

using Clock_t = std::chrono::steady_clock;
using namespace std::chrono_literals;

std::mutex g_mutex{};
std::condition_variable g_cv{};
std::vector<EventTypeEnum> g_events{};

void resetEvents()
{
    std::unique_lock<std::mutex> lock(g_mutex);
    g_events.clear();
}

bool waitForCount(const size_t expectedCount, const std::chrono::milliseconds timeout)
{
    std::unique_lock<std::mutex> lock(g_mutex);
    return g_cv.wait_for(lock, timeout, [expectedCount]() {
        return g_events.size() >= expectedCount;
    });
}

std::vector<EventTypeEnum> snapshotEvents()
{
    std::unique_lock<std::mutex> lock(g_mutex);
    return g_events;
}
} // namespace

void onExpired(std::vector<EventTypeEnum> events)
{
    {
        std::unique_lock<std::mutex> lock(g_mutex);
        g_events.insert(g_events.end(), events.begin(), events.end());
    }
    g_cv.notify_all();
}

TEST(EventSystemTimerTests, SimpleWaitForTimerCallback)
{
    using namespace std::chrono;
    resetEvents();

    EventSystemTimer timer(onExpired);

    timer.init();
    const auto startedAt = Clock_t::now();
    timer.startTimer(EventTypeEnum::eTimerA, 30ms);

    const bool fired = waitForCount(1, 500ms);
    const auto stoppedAt = Clock_t::now();
    ASSERT_TRUE(fired);

    const auto events = snapshotEvents();
    ASSERT_EQ(events.size(), 1u);

    const auto elapsed = duration_cast<milliseconds>(stoppedAt - startedAt);
    std::cout << "[Simple] expected=30ms actual=" << elapsed.count() << "ms\n";
    EXPECT_EQ(events.front(), EventTypeEnum::eTimerA);
    EXPECT_GE(elapsed.count(), 10);
    EXPECT_LE(elapsed.count(), 300);

    timer.shutdown();
}

TEST(EventSystemTimerTests, SecondTimerWithLaterDeadlineFiresAfterFirst)
{
    resetEvents();
    EventSystemTimer timer(onExpired);
    timer.init();

    timer.startTimer(EventTypeEnum::eTimerA, 120ms);
    std::this_thread::sleep_for(20ms);
    timer.startTimer(EventTypeEnum::eTimerB, 200ms);

    ASSERT_TRUE(waitForCount(2, 1000ms));
    const auto events = snapshotEvents();
    ASSERT_GE(events.size(), 2u);
    EXPECT_EQ(events[0], EventTypeEnum::eTimerA);
    EXPECT_EQ(events[1], EventTypeEnum::eTimerB);

    timer.shutdown();
}

TEST(EventSystemTimerTests, SecondTimerWithEarlierDeadlineFiresBeforeFirst)
{
    resetEvents();
    EventSystemTimer timer(onExpired);
    timer.init();

    timer.startTimer(EventTypeEnum::eTimerA, 220ms);
    std::this_thread::sleep_for(20ms);
    timer.startTimer(EventTypeEnum::eTimerB, 40ms);

    ASSERT_TRUE(waitForCount(2, 1000ms));
    const auto events = snapshotEvents();
    ASSERT_GE(events.size(), 2u);
    EXPECT_EQ(events[0], EventTypeEnum::eTimerB);
    EXPECT_EQ(events[1], EventTypeEnum::eTimerA);

    timer.shutdown();
}

TEST(EventSystemTimerTests, TwoTimersWithSameDueDateBothFire)
{
    resetEvents();
    EventSystemTimer timer(onExpired);
    timer.init();

    timer.startTimer(EventTypeEnum::eTimerA, 150ms);
    std::this_thread::sleep_for(20ms);
    timer.startTimer(EventTypeEnum::eTimerB, 130ms);

    ASSERT_TRUE(waitForCount(2, 1000ms));
    const auto events = snapshotEvents();
    ASSERT_GE(events.size(), 2u);

    const auto hasA = std::find(events.begin(), events.end(), EventTypeEnum::eTimerA) != events.end();
    const auto hasB = std::find(events.begin(), events.end(), EventTypeEnum::eTimerB) != events.end();
    EXPECT_TRUE(hasA);
    EXPECT_TRUE(hasB);

    timer.shutdown();
}

TEST(EventSystemTimerTests, MultipleTimersAreAllTriggeredInDueOrder)
{
    resetEvents();
    EventSystemTimer timer(onExpired);
    timer.init();

    timer.startTimer(EventTypeEnum::eTimerA, 70ms);
    timer.startTimer(EventTypeEnum::eTimerB, 120ms);
    timer.startTimer(EventTypeEnum::eTimerC, 30ms);
    timer.startTimer(EventTypeEnum::eTimerD, 160ms);

    ASSERT_TRUE(waitForCount(4, 1000ms));
    const auto events = snapshotEvents();
    ASSERT_GE(events.size(), 4u);
    EXPECT_EQ(events[0], EventTypeEnum::eTimerC);
    EXPECT_EQ(events[1], EventTypeEnum::eTimerA);
    EXPECT_EQ(events[2], EventTypeEnum::eTimerB);
    EXPECT_EQ(events[3], EventTypeEnum::eTimerD);

    timer.shutdown();
}

TEST(EventSystemTimerTests, TimeoutDistributionHasReasonableMinAndMax)
{
    using namespace std::chrono;

    EventSystemTimer timer(onExpired);
    timer.init();

    std::vector<long long> elapsedValuesMs{};
    elapsedValuesMs.reserve(20);

    constexpr auto expectedTimeout = 30ms;
    for (size_t i = 0; i < 20; ++i)
    {
        resetEvents();
        const auto startedAt = Clock_t::now();
        timer.startTimer(EventTypeEnum::eTimerA, expectedTimeout);
        ASSERT_TRUE(waitForCount(1, 800ms));
        const auto stoppedAt = Clock_t::now();

        const auto elapsed = duration_cast<milliseconds>(stoppedAt - startedAt);
        elapsedValuesMs.push_back(elapsed.count());
        std::cout << "[Distribution] sample=" << (i + 1) << " expected="
                  << expectedTimeout.count() << "ms actual=" << elapsed.count() << "ms\n";
        std::this_thread::sleep_for(5ms);
    }

    const auto [minIt, maxIt] = std::minmax_element(elapsedValuesMs.begin(), elapsedValuesMs.end());
    ASSERT_NE(minIt, elapsedValuesMs.end());
    ASSERT_NE(maxIt, elapsedValuesMs.end());

    const auto minElapsed = *minIt;
    const auto maxElapsed = *maxIt;
    std::cout << "[Distribution] min=" << minElapsed << "ms max=" << maxElapsed
              << "ms expected=" << expectedTimeout.count() << "ms\n";

    // Best-effort sanity bounds for non-realtime systems in CI/debug conditions.
    EXPECT_GE(minElapsed, 10);
    EXPECT_LE(maxElapsed, 300);

    timer.shutdown();
}

} // namespace event_system
