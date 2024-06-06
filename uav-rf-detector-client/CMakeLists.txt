cmake_minimum_required(VERSION 3.27)

project(UAVDetectorClient
        LANGUAGES CXX
)
set(USER_PROJECT_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/conan.cmake)
conan_cmake_run(CONANFILE conanfile.py
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing)

add_subdirectory(app)