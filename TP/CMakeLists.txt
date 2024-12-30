cmake_minimum_required(VERSION 3.29)
project(TP)

set(CMAKE_CXX_STANDARD 20)

add_executable(TP main.cpp
        GameData.cpp
        headers/GameData.h
        Player.cpp
        headers/Player.h
        Caravana.cpp
        headers/Caravana.h
        Item.cpp
        headers/Item.h
        Buffer.cpp
        headers/Buffer.h
        Cidade.cpp
        headers/Cidade.h
        mapManager.cpp
        headers/mapManager.h
)
