#pragma once
#include <cstdint>
#include <chrono>

namespace event_system {
namespace dynamic {
namespace events {

struct Timestamp 
{
    uint32_t counter{};
    std::chrono::time_point<std::chrono::system_clock> timestamp{};
    bool fromCb{false};
};

struct Speed
{
    enum class Unit : uint8_t
    {
        eUnknown = 0,
        eKmh,
        eMph,
    };
    
    float speed{};
    Unit unit{Unit::eUnknown};
};

}
}
}