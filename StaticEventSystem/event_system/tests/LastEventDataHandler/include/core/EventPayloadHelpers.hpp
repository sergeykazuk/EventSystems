#pragma once
#include "TestEventTypeEnum.hpp"
#include "TestPayloads.hpp"
#include <utility>

namespace event_system
{

    void destroyEventPayload(const EventTypeEnum eventType, std::byte *storage)
    {
        if (storage == nullptr)
        {
            return;
        }

        switch (eventType)
        {
        case EventTypeEnum::U8:
        {
            auto *object = reinterpret_cast<PayloadU8 *>(storage);
            object->~PayloadU8();
            break;
        }
        case EventTypeEnum::U16Int:
        {
            auto *object = reinterpret_cast<PayloadU16Int *>(storage);
            object->~PayloadU16Int();
            break;
        }
        case EventTypeEnum::String:
        {
            auto *object = reinterpret_cast<PayloadString *>(storage);
            object->~PayloadString();
            break;
        }
        case EventTypeEnum::BoolInt:
        {
            auto *object = reinterpret_cast<PayloadBoolInt *>(storage);
            object->~PayloadBoolInt();
            break;
        }
        case EventTypeEnum::Mixed:
        {
            auto *object = reinterpret_cast<PayloadMixed *>(storage);
            object->~PayloadMixed();
            break;
        }
        default:
            break;
        }
    }
}
