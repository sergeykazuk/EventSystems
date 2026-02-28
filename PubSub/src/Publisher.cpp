#include "Publisher.hpp"

namespace PubSub {

    void Publisher::addUserUpdateCallback(UserUpdatedCB_t cb)
    {
        m_userUpdateCallbacks.push_back(cb);
    }

    void Publisher::addNewOrderCallback(NewOrderCB_t cb)
    {
        m_newOrderCallbacks.push_back(cb);
    }

    void Publisher::addedNewUser(const User& newUser) const
    {
        for (auto cb: m_userUpdateCallbacks)
        {
            cb(newUser);
        }
    }

    void Publisher::addedNewOrder(const Order& newOrder) const
    {
        for (auto cb: m_newOrderCallbacks)
        {
            cb(newOrder);
        }
    }

}