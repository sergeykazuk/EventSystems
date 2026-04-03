#include "gtest/gtest.h"
#include "core/EventPayloadEraser.hpp"
#include <cstdint>
#include <string>

namespace {
using namespace event_system;


struct TrivialPayload {
    int a{};
    int b{};
};

struct TrackedPayload {
    static inline int liveInstances = 0;

    std::string text{};
    int32_t signedValue{};
    uint32_t unsignedValue{};
    bool enabled{false};
    bool& destructorCalled;

    TrackedPayload(std::string textValue,
                   int32_t signedV,
                   uint32_t unsignedV,
                   bool enabledValue,
                   bool& destructorFlag
                   )
        : text(std::move(textValue))
        , signedValue(signedV)
        , unsignedValue(unsignedV)
        , enabled(enabledValue)
        , destructorCalled(destructorFlag)
    {
        ++liveInstances;
    }

    TrackedPayload(const TrackedPayload& other)
        : text(other.text)
        , signedValue(other.signedValue)
        , unsignedValue(other.unsignedValue)
        , enabled(other.enabled)
        , destructorCalled(other.destructorCalled)
    {
        ++liveInstances;
    }

    ~TrackedPayload() {
        destructorCalled = true;
        --liveInstances;
    }
};

TEST(EventPayloadEraserTest, TrivialPayloadIsCopiedIntoByteStorage)
{
    const TrivialPayload source{17, -4};

    BytePtr_t erased = erasePayload(source);

    const auto* restored = reinterpret_cast<const TrivialPayload*>(erased.get());
    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(restored->a, source.a);
    EXPECT_EQ(restored->b, source.b);
}

TEST(EventPayloadEraserTest, NonTrivialPayloadDestructorRunsOnBytePtrReset)
{
    bool erasedPayloadDestroyed{false};
    TrackedPayload source{
        "payload",
        -42,
        420U,
        true,
        erasedPayloadDestroyed
    };

    ASSERT_EQ(TrackedPayload::liveInstances, 1);
    ASSERT_FALSE(erasedPayloadDestroyed);

    {
        BytePtr_t erased = erasePayload(source);
        ASSERT_EQ(TrackedPayload::liveInstances, 2);
        ASSERT_FALSE(erasedPayloadDestroyed);

        const auto* restored = reinterpret_cast<const TrackedPayload*>(erased.get());
        ASSERT_NE(restored, nullptr);
        EXPECT_EQ(restored->text, source.text);
        EXPECT_EQ(restored->signedValue, source.signedValue);
        EXPECT_EQ(restored->unsignedValue, source.unsignedValue);
        EXPECT_EQ(restored->enabled, source.enabled);

    }
    EXPECT_TRUE(erasedPayloadDestroyed);
    EXPECT_EQ(TrackedPayload::liveInstances, 1);
}

TEST(EventPayloadEraserTest, TwoTrackedPayloadErasuresFromSameSourceAreEqualAndIndependent)
{
    bool destructorCalled{false};
    TrackedPayload source{
        "same-source",
        -101,
        9001U,
        true,
        destructorCalled
    };

    {
        BytePtr_t erasedA = erasePayload(source);
        BytePtr_t erasedB = erasePayload(source);

        ASSERT_NE(erasedA.get(), nullptr);
        ASSERT_NE(erasedB.get(), nullptr);
        ASSERT_NE(erasedA.get(), erasedB.get());

        auto* payloadA = reinterpret_cast<TrackedPayload*>(erasedA.get());
        auto* payloadB = reinterpret_cast<TrackedPayload*>(erasedB.get());

        ASSERT_NE(payloadA, nullptr);
        ASSERT_NE(payloadB, nullptr);

        EXPECT_EQ(payloadA->text, payloadB->text);
        EXPECT_EQ(payloadA->signedValue, payloadB->signedValue);
        EXPECT_EQ(payloadA->unsignedValue, payloadB->unsignedValue);
        EXPECT_EQ(payloadA->enabled, payloadB->enabled);
        EXPECT_EQ(TrackedPayload::liveInstances, 3);

        payloadA->text = "changed";
        payloadA->signedValue = 42;
        payloadA->unsignedValue = 42U;
        payloadA->enabled = false;

        EXPECT_EQ(payloadB->text, source.text);
        EXPECT_EQ(payloadB->signedValue, source.signedValue);
        EXPECT_EQ(payloadB->unsignedValue, source.unsignedValue);
        EXPECT_EQ(payloadB->enabled, source.enabled);
    }
    EXPECT_EQ(TrackedPayload::liveInstances, 1);
}

} // namespace
