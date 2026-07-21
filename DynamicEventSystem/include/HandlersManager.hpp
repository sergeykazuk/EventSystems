#pragma once
#include "typedefs.hpp"
#include "TypeEraser.hpp"
#include <unordered_map>
#include <vector>
#include <functional>
#include <type_traits>

namespace event_system {
namespace dynamic {

class HandlersManager
{
using CB_t = std::function<void(const EventBufferView&)>;

public:
    template<typename T>
    void registerCallback(std::function<void(const T&)> cb)
    {
        using StoredT = std::decay_t<T>;
        static_assert(std::is_trivially_copyable_v<StoredT>,
            "DynamicEventSystem callbacks require trivially copyable payloads");

        auto typeId = TypeID::value<T>();
        
        auto cbWrapper = [callback = std::move(cb)](const EventBufferView& view) {
            if (view.data != nullptr && view.size == sizeof(StoredT))
            {
                const auto* eventPtr = reinterpret_cast<const StoredT*>(view.data);
                const StoredT& event = *eventPtr;
                callback(event);
            }
        };

        m_evCallbacks[typeId].push_back(std::move(cbWrapper));
    }

    void dispatchEvent(const TypeID_t id, const EventBufferView& data)
    {
        auto it = m_evCallbacks.find(id);

        if (it == m_evCallbacks.end())
        {
            return;
        }

        for (auto& cb: it->second)
        {
            cb(data);
        }
    }

private:

    using CbWrapper = std::function<void(const EventBufferView&)>;
    using EvCallbacks = std::vector<CbWrapper>;

    std::unordered_map<TypeID_t, EvCallbacks> m_evCallbacks{};
};

}
}