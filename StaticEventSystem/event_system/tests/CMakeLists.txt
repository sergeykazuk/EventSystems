include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(event_system_timer_tests
    EventSystemTimerTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/EventSystemTimer.cpp
)

target_include_directories(event_system_timer_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(event_system_timer_tests
    PRIVATE
        gtest_main
)

target_compile_features(event_system_timer_tests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(event_system_timer_tests)
