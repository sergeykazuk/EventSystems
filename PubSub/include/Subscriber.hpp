#pragma once

#include <unordered_map>
#include <string>

namespace PubSub {

struct IPublisher;
struct User;
struct Order;

class Subscriber {
public:
    explicit Subscriber(IPublisher& publisher);

private:
    void onUserAdded(const User&);
    void onOrderAdded(const Order&);

private:
    std::unordered_map<size_t, std::string> m_names{};
};

}