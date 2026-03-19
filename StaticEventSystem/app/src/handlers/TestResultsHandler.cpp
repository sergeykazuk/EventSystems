#include "app/handlers/TestResultsHandler.hpp"
#include "system/EventSystemHelpers.hpp"
#include "types/BoolFlags.hpp"
#include <iostream>

namespace app
{

TestResultsHandler::TestResultsHandler()
{
    const auto result = event_system::registerEventHandler(getEventHandlerId(), this);
    if (result != event_system::EventSystemOperationResult::eSuccess)
    {
        std::cerr << "[TestResultsHandler] registerEventHandler failed, result="
                  << event_system::toString(result) << "\n";
    }
}

TestResultsHandler::~TestResultsHandler()
{
    const auto result = event_system::unregisterEventHandler(getEventHandlerId());
    if (result != event_system::EventSystemOperationResult::eSuccess)
    {
        std::cerr << "[TestResultsHandler] unregisterEventHandler failed, result="
                  << event_system::toString(result) << "\n";
    }
}

void TestResultsHandler::handleEventTestSuccessful(const event_system::BoolFlags& event)
{
    std::cout << "[TestResultsHandler] TestSuccessful: "
         << event_system::toString(event) << "\n";
}

void TestResultsHandler::handleEventEventSystemShutdown()
{
    std::cout << "[TestResultsHandler] EventSystemShutdown\n";
}

}
