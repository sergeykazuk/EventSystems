#pragma once
#include <cstdint>

// EventTypeEnum for test events
namespace event_system {
enum class EventTypeEnum : uint16_t {
    U8 = 0,
    U16Int = 1,
    String = 2,
    BoolInt = 3,
    Mixed = 4,
    eCount
};
}