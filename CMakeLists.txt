cmake_minimum_required(VERSION 3.15)
project(PixelStream)

set(CMAKE_CXX_STANDARD 17)

# Locate packages via vcpkg
find_package(LibDataChannel REQUIRED)
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(src)

# Define sources
set(SOURCES 
    src/main.cpp
    src/Network/SignalingClient.cpp
    src/Network/SignalingClient.hpp
    src/Network/WebRTCManager.cpp
    src/Network/WebRTCManager.hpp
    src/SimEngine/PhysicsEngine.cpp
    src/SimEngine/PhysicsEngine.hpp
)

add_executable(PixelStream ${SOURCES})
target_include_directories(PixelStream PRIVATE ${CMAKE_SOURCE_DIR}/src)
# Link the libraries
target_link_libraries(PixelStream PRIVATE LibDataChannel::LibDataChannel nlohmann_json::nlohmann_json)