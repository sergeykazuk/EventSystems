#pragma once

#include "app/handlers/EventHandler1.hpp"
#include "app/handlers/EventHandler2.hpp"
#include "app/handlers/TestResultsHandler.hpp"
#include <atomic>

namespace app
{

class App final
{
public:
    App() = default;
    ~App() = default;

    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(App&&) = delete;
    App& operator=(App&&) = delete;

    int run();

private:    
    void onSignal(const int signal);

private:

    EventHandler1 m_handler1{};
    EventHandler2 m_handler2{};
    TestResultsHandler m_testResultsHandler{};
};

}
