cmake_minimum_required(VERSION 3.14)
project(EventSystemTimerTests)

include(GoogleTest)

add_executable(event_system_timer_tests
    EventSystemTimerTests.cpp
    ../../src/core/EventSystemTimer.cpp
)

target_include_directories(event_system_timer_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(event_system_timer_tests
    PRIVATE
        gtest_main
)

target_compile_features(event_system_timer_tests PRIVATE cxx_std_17)

gtest_discover_tests(event_system_timer_tests)
