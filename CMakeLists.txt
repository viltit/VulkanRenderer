cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

project(moe)

# Compile the shaders
execute_process(
        COMMAND bash compile.sh
)

add_subdirectory(vkRenderer)
add_subdirectory(Exceptions)

# Staticly link spdlog, a logger library for C++
# See https://github.com/gabime/spdlog
add_subdirectory(external/spdlog)

add_executable(vkEngine main.cpp
        Timer.hpp
        Colors/ColorRGB.hpp)

target_link_libraries(vkEngine
        PUBLIC spdlog
        vkRenderer
        moeExceptions)