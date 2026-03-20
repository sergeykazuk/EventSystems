cmake_minimum_required(VERSION 3.20)

project(StaticEventSystemWorkspace LANGUAGES CXX)

include(CTest)

add_subdirectory(event_system)
add_subdirectory(app)
