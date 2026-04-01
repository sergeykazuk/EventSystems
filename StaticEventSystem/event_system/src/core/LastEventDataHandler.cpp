#include "core/LastEventDataHandler.hpp"
#include "core/EventPayloadHelpers.hpp"
#include "core/LastEventHelpers.ipp"
#include <bitset>
#include <array>
#include <mutex>

namespace event_system {

struct LastEventDataHandler::ClassData
{
    std::bitset<static_cast<size_t>(EventTypeEnum::eCount)> m_availableLastEvents{};
    std::array<std::byte, calculateLastEventPayloadsSize()> m_lastEventsData{};
    std::mutex m_mutex;
};


LastEventDataHandler::LastEventDataHandler()
    : m_pimpl(std::make_unique<ClassData>())
{
}

LastEventDataHandler::~LastEventDataHandler()
{
    // Use generated destroyEventPayload for each present event
    for (size_t i = 0; i < static_cast<size_t>(EventTypeEnum::eCount); ++i) {
        if (m_pimpl->m_availableLastEvents.test(i)) {
            auto offset = getLastEventOffset(static_cast<EventTypeEnum>(i)).lastEventOffset;
            auto ptr = reinterpret_cast<std::byte*>(&m_pimpl->m_lastEventsData[offset]);
            destroyEventPayload(static_cast<EventTypeEnum>(i), ptr);
        }
    }
}


void LastEventDataHandler::updateLastEventData(const EventTypeEnum ev, BytePtr_t data)
{
    if (!data)
        return;

    auto [offset, size] = getLastEventOffset(ev);

    // Move bytes from data.get() into the array (swap)
    std::unique_lock<std::mutex> lock(m_pimpl->m_mutex);
    for (size_t i = 0; i < size; ++i) {
        std::swap(m_pimpl->m_lastEventsData[offset + i], data[i]);
    }
    // now data is owned by last event data handler
    m_pimpl->m_availableLastEvents.set(static_cast<size_t>(ev), true);
}

bool LastEventDataHandler::getLastEventData(const EventTypeEnum eventId,
        const std::function<void(std::byte const * const)>& visitor) const
{
    std::unique_lock<std::mutex> lock(std::mutex);

    if (!m_pimpl->m_availableLastEvents.test(static_cast<size_t>(eventId)))
        return false;

    auto offset = getLastEventOffset(eventId).lastEventOffset;
    std::byte* ptr = reinterpret_cast<std::byte*>(&m_pimpl->m_lastEventsData[offset]);
    visitor(ptr);
    return true;
}


}