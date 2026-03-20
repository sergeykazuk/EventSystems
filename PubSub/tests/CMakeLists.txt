include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Keep GoogleTest options consistent and quiet in subprojects.
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(pubsub_unit_tests
    PublisherTests.cpp
    SubscriberTests.cpp
    CsvDataReaderLifecycleTests.cpp
)

target_link_libraries(pubsub_unit_tests
    PRIVATE
        PubSub
        gtest_main
)

target_include_directories(pubsub_unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

include(GoogleTest)
gtest_discover_tests(pubsub_unit_tests)
