#pragma once
#include "IPublisher.hpp"
#include <list>

namespace PubSub {

struct User;
struct Order;

class Publisher: public IPublisher
{
public:

    void addUserUpdateCallback(UserUpdatedCB_t cb) override;
    void addNewOrderCallback(NewOrderCB_t cb) override;

    void addedNewUser(const User& newUser) const;
    void addedNewOrder(const Order& newOrder) const;

private:
    std::list<UserUpdatedCB_t> m_userUpdateCallbacks{};
    std::list<NewOrderCB_t> m_newOrderCallbacks{};   
};

}