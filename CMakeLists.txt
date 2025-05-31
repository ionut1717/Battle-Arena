cmake_minimum_required(VERSION 3.5)
project(Battle-Arena LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(Battle-Arena
        Battle_Arena.cpp
        Arena.h
        Balloon.h
        Game.h
        Player.h
        Tiles.h
        Balloon.cpp
        Resource_Manager.cpp
        Resource_Manager.h
        Tiles.cpp
        Player.cpp
        Game.cpp
        Arena.cpp
)
target_compile_features(Battle-Arena PRIVATE cxx_std_17)
target_link_libraries(Battle-Arena PRIVATE SFML::Graphics SFML::Window SFML::System)