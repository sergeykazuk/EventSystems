// This is a custom .ipp file for test purposes
// It should be included by LastEventDataHandler.cpp in test builds
// It provides pool initialization for the test event types
#include "TestEventTypeEnum.hpp"
#include "TestPayloads.hpp"
#include <cstddef>
#include <type_traits>

namespace event_system {

constexpr size_t getEventPayloadSize(EventTypeEnum ev) {
    switch (ev) {
        case EventTypeEnum::U8: return sizeof(PayloadU8);
        case EventTypeEnum::U16Int: return sizeof(PayloadU16Int);
        case EventTypeEnum::String: return sizeof(PayloadString);
        case EventTypeEnum::BoolInt: return sizeof(PayloadBoolInt);
        case EventTypeEnum::Mixed: return sizeof(PayloadMixed);
        default: return 0;
    }
}

constexpr size_t calculateLastEventPayloadsSize() {
    return sizeof(PayloadU8) + sizeof(PayloadU16Int) + sizeof(PayloadString) + sizeof(PayloadBoolInt) + sizeof(PayloadMixed);
}

struct LastEventOffset {
    size_t lastEventOffset;
    size_t lastEventSize;
};

constexpr LastEventOffset getLastEventOffset(EventTypeEnum ev) {
    size_t offset = 0;
    switch (ev) {
        case EventTypeEnum::U8:
            return {offset, sizeof(PayloadU8)};
        case EventTypeEnum::U16Int:
            offset += sizeof(PayloadU8);
            return {offset, sizeof(PayloadU16Int)};
        case EventTypeEnum::String:
            offset += sizeof(PayloadU8) + sizeof(PayloadU16Int);
            return {offset, sizeof(PayloadString)};
        case EventTypeEnum::BoolInt:
            offset += sizeof(PayloadU8) + sizeof(PayloadU16Int) + sizeof(PayloadString);
            return {offset, sizeof(PayloadBoolInt)};
        case EventTypeEnum::Mixed:
            offset += sizeof(PayloadU8) + sizeof(PayloadU16Int) + sizeof(PayloadString) + sizeof(PayloadBoolInt);
            return {offset, sizeof(PayloadMixed)};
        default:
            return {0, 0};
    }
}

} // namespace event_system
