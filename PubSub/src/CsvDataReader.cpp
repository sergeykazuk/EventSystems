#include "CsvDataReader.hpp"
#include "Publisher.hpp"
#include "DataTypes.hpp"
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <functional>
#include <iostream>
#include <mutex>
#include <condition_variable>

namespace {
    constexpr std::chrono::seconds READ_PERIOD{2};
    constexpr char const * const USERS_FILE{"Data/users.csv"};
    constexpr char const * const ORDER_FILE{"Data/oders.csv"};

    template<typename T>
    using Parser_t = std::function<std::optional<T>(std::string)>;

    template<typename T>
    std::vector<T> readNewRecords(const char* fileName, Parser_t<T> parser)
    {
        static size_t count = 0;
        std::vector<T> out;

        std::ifstream in(fileName);
        if (!in) {
            std::cout << "Cannot open file '" << fileName << "'\n";
            return out;
        }

        std::string line;
        size_t current = 0;

        // skip lines we've already processed (they were complete when seen)
        while (current < count) {
            if (!std::getline(in, line)) {
                count = 0;
                return out;
            }
            ++current;
        }

        // read new lines; stop on the first incomplete (no semicolon)
        while (std::getline(in, line)) {
            if (line.empty() || line.find(';') == std::string::npos) {
                break;
            }

            auto maybe = parser(line);
            if (maybe) {
                out.push_back(*maybe);
            }
        }

        count += out.size();
        return out;
    }

}

namespace PubSub {

struct CsvDataReader::PimplData {
    
    PimplData(const Publisher& publisher)
        : m_publisher(publisher)
    {}
    ~PimplData()
    {
        if (m_thread && m_thread->joinable()) {
            m_thread->join();
        }
    }

    const Publisher& m_publisher;
    bool m_shouldStop{false};
    std::unique_ptr<std::thread> m_thread{};

    // needed to make thread wait between sleeps
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_isRunning{false};
};

CsvDataReader::CsvDataReader(const Publisher& publisher)
    : IDataReader()
    , m_pimpl(std::make_unique<PimplData>(publisher))
{}

CsvDataReader::~CsvDataReader()
{
}

void CsvDataReader::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_pimpl->m_mutex);
        m_pimpl->m_shouldStop = true;
    }
    m_pimpl->m_cv.notify_one();

    while(m_pimpl->m_isRunning);
}

bool CsvDataReader::isRunning() const
{
    return m_pimpl->m_isRunning;
}

void CsvDataReader::runner()
{
    // Parser for User: expects "name;" format
    // Strips the semicolon and returns the name as the user
    auto userParser = [](const std::string& line) -> std::optional<User> {
        // find and remove the semicolon
        size_t semiPos = line.find(';');
        if (semiPos == std::string::npos) {
            return std::nullopt;
        }
        std::string name = line.substr(0, semiPos);
        if (name.empty()) {
            return std::nullopt;
        }
        static size_t sUserId{};
        User u{++sUserId, name};
        return u;
    };

    // Parser for Order: expects "userId,description;" format
    // Strips the semicolon, parses CSV, and auto-assigns orderId
    auto orderParser = [](const std::string& line) -> std::optional<Order> {
        // find and remove the semicolon
        size_t semiPos = line.find(';');
        if (semiPos == std::string::npos) {
            return std::nullopt;
        }
        std::string content = line.substr(0, semiPos);
        if (content.empty()) {
            return std::nullopt;
        }

        std::istringstream iss(content);
        Order o{};
        static size_t sOrderId{};

        if ((iss >> o.m_userId) && iss.get() == ',' &&
            std::getline(iss, o.m_details)) {
            o.m_orderId = ++sOrderId;
            return o;
        }
        return std::nullopt;
    };

    while (!m_pimpl->m_shouldStop) {
        // collect any new users in one shot
        auto users = readNewRecords<User>(USERS_FILE, userParser);
        for (const auto& u : users) {
            m_pimpl->m_publisher.addedNewUser(u);
        }

        // and orders likewise
        auto orders = readNewRecords<Order>(ORDER_FILE, orderParser);
        for (const auto& o : orders) {
            m_pimpl->m_publisher.addedNewOrder(o);
        }

        std::unique_lock<std::mutex> lock(m_pimpl->m_mutex);
        m_pimpl->m_cv.wait_for(lock, READ_PERIOD, [this](){ return m_pimpl->m_shouldStop; });
    }
    m_pimpl->m_isRunning = false;
}

void CsvDataReader::startReadingData()
{
    auto fileExists = [](const char* fileName) {
        auto exists = std::filesystem::exists(fileName);
        std::cout << "File '" << fileName << "' " 
                  << (exists ? "exists" : "does not exists")
                  << std::endl;
        return exists;
    };

    bool canStart = fileExists(USERS_FILE) && fileExists(ORDER_FILE);

    if (canStart)
    {
        m_pimpl->m_thread = std::make_unique<std::thread>([this]() {
            runner();
        });
        m_pimpl->m_isRunning = true;
    }
    else 
    {
        std::cout << "cannot start since config files do not exist\n";
    }
}

}
