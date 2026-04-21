cmake_minimum_required(VERSION 3.10)
project(FlappyBird)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(flappy-bird src/main.cpp)

target_link_libraries(flappy-bird PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
