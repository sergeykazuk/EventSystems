#pragma once
#include <cstddef>
#include <cstdint>

namespace event_system {
namespace dynamic {

using TypeID_t = std::size_t;
using EventId = std::uint16_t;

struct EventBufferView
{
	const std::byte* data{nullptr};
	std::size_t size{0};
};

}
}