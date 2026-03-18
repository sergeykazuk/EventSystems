cmake_minimum_required(VERSION 3.20)

project(StaticEventSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(STATIC_EVENT_SYSTEM_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(STATIC_EVENT_SYSTEM_GENERATE_SCRIPT ${STATIC_EVENT_SYSTEM_ROOT}/scripts/generate_all.py)

execute_process(
    COMMAND ${Python3_EXECUTABLE} ${STATIC_EVENT_SYSTEM_GENERATE_SCRIPT}
    WORKING_DIRECTORY ${STATIC_EVENT_SYSTEM_ROOT}
    RESULT_VARIABLE STATIC_EVENT_SYSTEM_GEN_RESULT
)

if(NOT STATIC_EVENT_SYSTEM_GEN_RESULT EQUAL 0)
    message(FATAL_ERROR "StaticEventSystem code generation failed at configure time")
endif()

add_custom_target(static_event_system_generate
    COMMAND ${Python3_EXECUTABLE} ${STATIC_EVENT_SYSTEM_GENERATE_SCRIPT}
    WORKING_DIRECTORY ${STATIC_EVENT_SYSTEM_ROOT}
    COMMENT "Generating StaticEventSystem code"
    VERBATIM
)

file(GLOB_RECURSE STATIC_EVENT_SYSTEM_CORE_SOURCES CONFIGURE_DEPENDS
    ${STATIC_EVENT_SYSTEM_ROOT}/src/*.cpp
)

file(GLOB_RECURSE STATIC_EVENT_SYSTEM_GENERATED_SOURCES CONFIGURE_DEPENDS
    ${STATIC_EVENT_SYSTEM_ROOT}/generated/src/*.cpp
)

add_library(static_event_system STATIC
    ${STATIC_EVENT_SYSTEM_CORE_SOURCES}
    ${STATIC_EVENT_SYSTEM_GENERATED_SOURCES}
)

add_dependencies(static_event_system static_event_system_generate)

target_include_directories(static_event_system
    PUBLIC
        ${STATIC_EVENT_SYSTEM_ROOT}/include
        ${STATIC_EVENT_SYSTEM_ROOT}/generated/include
)
