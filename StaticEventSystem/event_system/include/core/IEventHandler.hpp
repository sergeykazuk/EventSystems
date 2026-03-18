#pragma once
#include "core/SystemTypes.hpp"

namespace event_system {

/// @brief Base interface for all event handlers
class IEventHandler 
{
public:
    IEventHandler() = default;
    virtual ~IEventHandler() = default;
    IEventHandler(const IEventHandler&) = default;
    IEventHandler(IEventHandler&&) = default;
    IEventHandler& operator=(const IEventHandler&) = default;
    IEventHandler& operator=(IEventHandler&&) noexcept = default;

    /// @brief This method will be implemented for each event handler individually
    ///        It will process generic events individually for each handler
    virtual void processEvent(const EventTypeEnum, const BytePtr_t&) = 0;
};

}