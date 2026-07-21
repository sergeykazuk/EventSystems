#pragma once
#include <atomic>
#include <type_traits>
#include "typedefs.hpp"

namespace event_system {
namespace dynamic {

class TypeID final
{
public:
    template<typename T>
    static TypeID_t value()
    {
        static TypeID_t id = counter++;
        return id;
    }

private:
    inline static std::atomic<TypeID_t> counter{0};
};

template<typename T>
constexpr bool isRegisteredPayloadType() noexcept
{
    using StoredT = std::decay_t<T>;
    return std::is_trivially_copyable_v<StoredT>;
}

}
}