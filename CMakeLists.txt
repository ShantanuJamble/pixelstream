cmake_minimum_required(VERSION 3.15)
project(PixelStream)

set(CMAKE_CXX_STANDARD 17)

# Locate packages via vcpkg
find_package(LibDataChannel REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(PixelStream src/main.cpp)

# Link the libraries
target_link_libraries(PixelStream PRIVATE LibDataChannel::LibDataChannel nlohmann_json::nlohmann_json)