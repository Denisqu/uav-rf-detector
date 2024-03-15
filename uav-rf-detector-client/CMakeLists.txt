cmake_minimum_required(VERSION 3.7)

project(UAVDetectorClient
        LANGUAGES CXX
)
set(USER_PROJECT_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(app/src)