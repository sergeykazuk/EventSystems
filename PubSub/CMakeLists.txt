cmake_minimum_required(VERSION 3.16)
project(PubSub VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# PubSub Library
add_library(PubSub STATIC
    src/Publisher.cpp
    src/CsvDataReader.cpp
    src/Subscriber.cpp
)

target_include_directories(PubSub
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Example executable
add_executable(PubSubExample main.cpp)
target_link_libraries(PubSubExample PRIVATE PubSub)

# Copy the entire Data directory into the build tree so the
# example executable can find the .txt files next to the binary.
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
