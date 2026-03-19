cmake_minimum_required(VERSION 3.16)
project(DynamicEventSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# DynamicEventSystem Library
add_library(DynEventSysLib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/EventQueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/EventSystem.cpp
)

target_include_directories(DynEventSysLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/events
)

target_compile_features(DynEventSysLib PUBLIC cxx_std_17)

# Example executable
add_executable(DynamicEventSystem main.cpp)
target_link_libraries(DynamicEventSystem PRIVATE DynEventSysLib)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
