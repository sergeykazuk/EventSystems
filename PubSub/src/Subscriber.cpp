#include "Subscriber.hpp"
#include "IPublisher.hpp"
#include "DataTypes.hpp"
#include <iostream>

namespace PubSub {

Subscriber::Subscriber(IPublisher& pub)
{
    pub.addNewOrderCallback([this](const Order& order){onOrderAdded(order); });
    pub.addUserUpdateCallback([this](const User& user){onUserAdded(user); });
}

void Subscriber::onOrderAdded(const Order& order)
{
    std::cout << order.m_orderId << " {user '" << m_names[order.m_userId]
              << "' ordered '" << order.m_details << "'}\n";
}

void Subscriber::onUserAdded(const User& user)
{
    m_names[user.m_userId] = user.m_name;
    std::cout << user.m_userId << " {name: " << user.m_name << "'}\n";

}

}