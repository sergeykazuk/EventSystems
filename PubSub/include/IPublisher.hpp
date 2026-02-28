#pragma once
#include <functional>

namespace PubSub {

struct User;
struct Order;

struct IPublisher
{
    IPublisher() = default;
    virtual ~IPublisher() = default;
    IPublisher(const IPublisher&) = delete;
    IPublisher(IPublisher&&) = default;
    IPublisher& operator= (const IPublisher&) = delete;
    IPublisher& operator= (IPublisher&&) noexcept = default;

    typedef std::function<void(const User&)> UserUpdatedCB_t;
    typedef std::function<void(const Order&)> NewOrderCB_t;

    virtual void addUserUpdateCallback(UserUpdatedCB_t cb) = 0;
    virtual void addNewOrderCallback(NewOrderCB_t cb) = 0;
};

}