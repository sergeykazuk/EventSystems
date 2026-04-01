#include "gtest/gtest.h"
#include "../../include/core/LastEventDataHandler.hpp"
#include "../../include/core/SystemTypes.hpp"
#include "TestPayloads.hpp"
#include "TestEventTypeEnum.hpp"
#include <cstring>
#include <type_traits>

using namespace event_system;

namespace {
template<typename T>
BytePtr_t erasePayload(T&& eventPayload)
{
    using EventPayloadT = std::decay_t<T>;

    auto deleter = [](std::byte* ptr) {
        reinterpret_cast<EventPayloadT*>(ptr)->~EventPayloadT();
        delete[] ptr;
    };

    std::unique_ptr<std::byte[], decltype(deleter)> 
            erased{new std::byte[sizeof(EventPayloadT)], deleter};

    if constexpr (std::is_trivially_copyable_v<EventPayloadT>)
    {
        std::memcpy(erased.get(), &eventPayload, sizeof(EventPayloadT));
        return erased;
    }

    new (erased.get()) EventPayloadT(std::forward<T>(eventPayload));
    return erased;
}

TEST(LastEventDataHandlerTest, StoreAndRetrieve_U8)
{
    LastEventDataHandler handler{};
    PayloadU8 payload{42};
    auto erased = erasePayload(payload);
    handler.updateLastEventData(EventTypeEnum::U8, std::move(erased));
    
    bool called = false;

    handler.getLastEventData(EventTypeEnum::U8, [&](const std::byte* data) {
        auto* restored = reinterpret_cast<const PayloadU8*>(data);
        std::cout << restored->value << std::endl;
        
        EXPECT_EQ(restored->value, payload.value);
        called = true;
    });
    EXPECT_TRUE(called);
}

TEST(LastEventDataHandlerTest, StoreAndRetrieve_U16Int)
{
    LastEventDataHandler handler{};
    PayloadU16Int payload{12345, -99};
    auto erased = erasePayload(payload);
    handler.updateLastEventData(EventTypeEnum::U16Int, std::move(erased));
    bool called = false;
    handler.getLastEventData(EventTypeEnum::U16Int, [&](const std::byte* data) {
        auto* restored = reinterpret_cast<const PayloadU16Int*>(data);
        EXPECT_EQ(restored->u16, payload.u16);
        EXPECT_EQ(restored->i32, payload.i32);
        called = true;
    });
    EXPECT_TRUE(called);
}

TEST(LastEventDataHandlerTest, StoreAndRetrieve_String)
{
    LastEventDataHandler handler{};
    PayloadString payload{"hello world"};
    auto erased = erasePayload(payload);
    handler.updateLastEventData(EventTypeEnum::String, std::move(erased));
    bool called = false;
    handler.getLastEventData(EventTypeEnum::String, [&](const std::byte* data) {
        auto* restored = reinterpret_cast<const PayloadString*>(data);
        EXPECT_EQ(restored->str, payload.str);
        called = true;
    });
    EXPECT_TRUE(called);
}

TEST(LastEventDataHandlerTest, StoreAndRetrieve_BoolInt)
{
    LastEventDataHandler handler{};
    PayloadBoolInt payload{true, 777};
    auto erased = erasePayload(payload);
    handler.updateLastEventData(EventTypeEnum::BoolInt, std::move(erased));
    bool called = false;
    handler.getLastEventData(EventTypeEnum::BoolInt, [&](const std::byte* data) {
        auto* restored = reinterpret_cast<const PayloadBoolInt*>(data);
        EXPECT_EQ(restored->flag, payload.flag);
        EXPECT_EQ(restored->number, payload.number);
        called = true;
    });
    EXPECT_TRUE(called);
}

TEST(LastEventDataHandlerTest, StoreAndRetrieve_Mixed)
{
    LastEventDataHandler handler{};
    PayloadMixed payload{255, "payload", false};
    auto erased = erasePayload(payload);
    handler.updateLastEventData(EventTypeEnum::Mixed, std::move(erased));
    bool called = false;
    handler.getLastEventData(EventTypeEnum::Mixed, [&](const std::byte* data) {
        auto* restored = reinterpret_cast<const PayloadMixed*>(data);
        EXPECT_EQ(restored->u8, payload.u8);
        EXPECT_EQ(restored->name, payload.name);
        EXPECT_EQ(restored->valid, payload.valid);
        called = true;
    });
    EXPECT_TRUE(called);
}

TEST(LastEventDataHandlerTest, StoreAllRetrieveSome)
{
    LastEventDataHandler handler{};

    PayloadMixed payload1{255, "payload", false};
    PayloadBoolInt payload2{true, 777};
    PayloadString payload3{"hello world"};
    PayloadU16Int payload4{12345, -99};
    PayloadU8 payload5{42};

    handler.updateLastEventData(EventTypeEnum::Mixed, erasePayload(payload1));
    handler.updateLastEventData(EventTypeEnum::BoolInt, erasePayload(payload2));
    handler.updateLastEventData(EventTypeEnum::String, erasePayload(payload3));
    handler.updateLastEventData(EventTypeEnum::U16Int, erasePayload(payload4));
    handler.updateLastEventData(EventTypeEnum::U8, erasePayload(payload5));

    payload4.i32 = 0xFF;
    payload4.u16 = 1;
    handler.updateLastEventData(EventTypeEnum::U16Int, erasePayload(payload4));

    bool called = false;
    handler.getLastEventData(EventTypeEnum::Mixed, [&](const std::byte* data) {
        auto* restored = reinterpret_cast<const PayloadMixed*>(data);
        EXPECT_EQ(restored->u8, payload1.u8);
        EXPECT_EQ(restored->name, payload1.name);
        EXPECT_EQ(restored->valid, payload1.valid);
        called = true;
    });

    handler.getLastEventData(EventTypeEnum::U16Int, [&](const std::byte* data) {
        auto* restored = reinterpret_cast<const PayloadU16Int*>(data);
        EXPECT_EQ(restored->i32, payload4.i32);
        EXPECT_EQ(restored->u16, payload4.u16);
        called = true;
    });

    EXPECT_TRUE(called);
}


} // namespace
