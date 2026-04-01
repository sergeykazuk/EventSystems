cmake_minimum_required(VERSION 3.14)
project(LastEventDataHandlerTests)

include(GoogleTest)

add_executable(last_event_data_handler_tests
    LastEventDataHandlerTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/core/LastEventDataHandler.cpp
)

target_include_directories(last_event_data_handler_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}                         
        ${CMAKE_CURRENT_SOURCE_DIR}/include                 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include                 
)

target_link_libraries(last_event_data_handler_tests
    PRIVATE
        gtest_main
)

target_compile_features(last_event_data_handler_tests PRIVATE cxx_std_17)

gtest_discover_tests(last_event_data_handler_tests)
