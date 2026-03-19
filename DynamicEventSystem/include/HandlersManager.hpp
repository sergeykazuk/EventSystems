#pragma once
#include "typedefs.hpp"
#include "TypeEraser.hpp"
#include <unordered_map>
#include <vector>
#include <functional>

namespace event_system {
namespace dynamic {

class HandlersManager
{
using CB_t = std::function<void(const BytePtr&)>;

public:
    template<typename T>
    void registerCallback(std::function<void(const T&)> cb)
    {
        auto typeId = TypeID::value<T>();
        
        auto cbWrapper = [callback = std::move(cb)](const BytePtr& data) {
            if (data)
            {
                const T& event = *(reinterpret_cast<T*>(data.get()));
                callback(event);
            }
        };

        m_evCallbacks[typeId].push_back(std::move(cbWrapper));
    }

    void dispatchEvent(const TypeID_t id, const BytePtr& data)
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

    using CbWrapper = std::function<void(const BytePtr&)>;
    using EvCallbacks = std::vector<CbWrapper>;

    std::unordered_map<TypeID_t, EvCallbacks> m_evCallbacks{};
};

}
}