cmake_minimum_required(VERSION 3.27)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
unset(CMAKE_GENERATOR_PLATFORM CACHE)
project(UAVDetectorClient
        LANGUAGES CXX
)
set(USER_PROJECT_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/conan.cmake)
conan_cmake_run(CONANFILE conanfile.py
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing
        SETTINGS build_type=${CMAKE_BUILD_TYPE})
include(build/generators/conan_toolchain.cmake)
unset(CMAKE_GENERATOR_PLATFORM CACHE)
add_subdirectory(app)