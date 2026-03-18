#include "app/handlers/TestResultsHandler.hpp"
#include "system/EventSystemHelpers.hpp"
#include "types/BoolFlags.hpp"
#include <iostream>

namespace app
{

TestResultsHandler::TestResultsHandler()
{
    event_system::registerEventHandler(getEventHandlerId(), this);
}

TestResultsHandler::~TestResultsHandler()
{
    event_system::unregisterEventHandler(getEventHandlerId());
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
