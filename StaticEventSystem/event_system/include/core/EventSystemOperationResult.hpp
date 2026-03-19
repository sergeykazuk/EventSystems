#pragma once
#include <cstdint>

namespace event_system {

enum class EventSystemOperationResult : uint8_t
{
    eSuccess = 0,
    eNullHandler,
    eInvalidState,
    eOutOfRangeHandlerId,
    eAlreadyRegistered,
    eNotRegistered
};

inline const char* toString(const EventSystemOperationResult result)
{
    switch (result)
    {
        case EventSystemOperationResult::eSuccess:
            return "eSuccess";
        case EventSystemOperationResult::eNullHandler:
            return "eNullHandler";
        case EventSystemOperationResult::eInvalidState:
            return "eInvalidState";
        case EventSystemOperationResult::eOutOfRangeHandlerId:
            return "eOutOfRangeHandlerId";
        case EventSystemOperationResult::eAlreadyRegistered:
            return "eAlreadyRegistered";
        case EventSystemOperationResult::eNotRegistered:
            return "eNotRegistered";
    }

    return "<unknown EventSystemOperationResult>";
}

}