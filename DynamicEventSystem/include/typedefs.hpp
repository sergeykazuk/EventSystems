#pragma once
#include <memory>
#include <cstddef>


namespace event_system {
namespace dynamic {

using BytePtr = std::unique_ptr<std::byte, void(*)(std::byte*)>;
using TypeID_t = size_t;

}
}