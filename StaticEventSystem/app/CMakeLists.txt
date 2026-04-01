cmake_minimum_required(VERSION 3.20)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
add_executable(test_app ${APP_SOURCES})

target_compile_features(test_app
    PRIVATE
        cxx_std_17
)

target_include_directories(test_app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(test_app
    PRIVATE
        static_event_system
)
