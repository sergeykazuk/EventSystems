#pragma once

#include "handlers/ITestResultsHandler.hpp"

namespace app
{

class TestResultsHandler final : protected event_system::ITestResultsHandler
{
public:
    TestResultsHandler();
    ~TestResultsHandler();

    TestResultsHandler(const TestResultsHandler&) = delete;
    TestResultsHandler& operator=(const TestResultsHandler&) = delete;
    TestResultsHandler(TestResultsHandler&&) = delete;
    TestResultsHandler& operator=(TestResultsHandler&&) = delete;

private:
    void handleEventTestSuccessful(const event_system::BoolFlags& event) override;
    void handleEventEventSystemShutdown() override;
};

}
