cmake_minimum_required(VERSION 3.14)
project(EventPayloadEraserTests)

include(GoogleTest)

add_executable(event_payload_eraser_tests
    EventPayloadEraserTests.cpp
)

target_include_directories(event_payload_eraser_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(event_payload_eraser_tests
    PRIVATE
        gtest_main
)

target_compile_features(event_payload_eraser_tests PRIVATE cxx_std_17)

gtest_discover_tests(event_payload_eraser_tests)
