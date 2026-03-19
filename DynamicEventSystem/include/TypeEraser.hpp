#pragma once
#include <atomic>
#include <new>
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
BytePtr createEvent(T&& ev)
{
    using StoredT = std::decay_t<T>;
    static_assert(std::is_constructible_v<StoredT, T&&>, "Stored type must be constructible from the provided argument");

    auto deleter = [](std::byte* p){
        if (p == nullptr)
        {
            return;
        }

        StoredT* casted = reinterpret_cast<StoredT*>(p);
        delete casted;
    };

    StoredT* raw = new (std::nothrow) StoredT(std::forward<T>(ev));
    if (raw == nullptr)
    {
        return BytePtr(nullptr, deleter);
    }
    return BytePtr(reinterpret_cast<std::byte*>(raw), deleter);
}

}
}