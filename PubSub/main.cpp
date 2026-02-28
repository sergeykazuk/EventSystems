#include "include/CsvDataReader.hpp"
#include "include/Publisher.hpp"
#include "include/Subscriber.hpp"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>

namespace {
std::atomic<bool> g_interrupted(false);

void signalHandler(int signal) {
    if (signal == SIGINT) {
        g_interrupted = true;
    }
}

}

int main()
{
    using namespace PubSub;

    // Register signal handler for Ctrl+C
    std::signal(SIGINT, signalHandler);
    {
        Publisher publisher;
        Subscriber subscriber(publisher);
        CsvDataReader reader(publisher);

        std::cout << "Starting CSV data reader...\n"
                << "Press Ctrl+C to stop.\n" << std::endl;

        reader.startReadingData();

        if (reader.isRunning())
        {
            // Wait until Ctrl+C is pressed
            while (!g_interrupted) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        std::cout << "\nShutting down..." << std::endl;
        reader.stop();
    }

    return 0;
}