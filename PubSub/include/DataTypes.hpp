#pragma once

#include <string>

namespace PubSub {

struct User {
    size_t m_userId{};
    std::string m_name{};
};

struct Order {
    size_t m_orderId{};
    size_t m_userId{};
    std::string m_details{};
};

}