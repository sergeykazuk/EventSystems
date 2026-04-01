#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>


namespace event_system {

enum class EventTypeEnum : uint16_t;
enum class EventHandlerId : uint16_t;
enum class EventQueueId : uint16_t;

class IEventHandler;
class EventDispatcher;
class EventQueue;
class EventSystem;

using BytePtr_t = std::unique_ptr<std::byte[], std::function<void(std::byte*)> >;
using HandlerIds_t = std::vector<EventHandlerId>;
using EventHandlersMap_t = std::unordered_map<EventTypeEnum, HandlerIds_t>;

}
