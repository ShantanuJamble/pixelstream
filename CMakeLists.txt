cmake_minimum_required(VERSION 3.15)
project(PixelStream)

set(CMAKE_CXX_STANDARD 17)

# Locate packages via vcpkg
find_package(LibDataChannel REQUIRED)
find_package(nlohmann_json REQUIRED)

# Search for gstreamer
if(WIN32)
    file(TO_CMAKE_PATH "$ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64}" GSTREAMER_ROOT)
    
    if(NOT GSTREAMER_ROOT)
        message(FATAL_ERROR "GStreamer env var not found! Did you restart your PC after install?")
    endif()

    message(STATUS "--- GStreamer Configuration ---")
    message(STATUS "Root: ${GSTREAMER_ROOT}")
   
    set(GST_INCLUDE_DIRS
        "${GSTREAMER_ROOT}/include/gstreamer-1.0"
        "${GSTREAMER_ROOT}/include/glib-2.0"
        "${GSTREAMER_ROOT}/lib/glib-2.0/include"
    )
    
    set(GST_LIB_DIR "${GSTREAMER_ROOT}/lib")
    
    message(STATUS "Includes: ${GST_INCLUDE_DIRS}")
    message(STATUS "Lib Dir: ${GST_LIB_DIR}")

    # Define the Libraries to Link
    set(GSTREAMER_LIBS
        "${GST_LIB_DIR}/gstreamer-1.0.lib"
        "${GST_LIB_DIR}/gobject-2.0.lib"
        "${GST_LIB_DIR}/glib-2.0.lib"
        "${GST_LIB_DIR}/gstapp-1.0.lib"
        "${GST_LIB_DIR}/gstvideo-1.0.lib"
    )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GST REQUIRED gstreamer-1.0 gstapp-1.0 gstvideo-1.0)
    set(GST_INCLUDE_DIRS ${GST_INCLUDE_DIRS})
    set(GSTREAMER_LIBS ${GST_LIBRARIES})
endif()


# Include directories
include_directories(src)
# Define sources
set(SOURCES 
    src/main.cpp
    src/Media/VideoPipeline.cpp
    src/Media/VideoPipeline.hpp
    src/Network/SignalingClient.cpp
    src/Network/SignalingClient.hpp
    src/Network/WebRTCManager.cpp
    src/Network/WebRTCManager.hpp
    src/SimEngine/PhysicsEngine.cpp
    src/SimEngine/PhysicsEngine.hpp
)

add_executable(PixelStream ${SOURCES})
# Link the libraries
target_link_libraries(PixelStream PRIVATE 
    LibDataChannel::LibDataChannel 
    nlohmann_json::nlohmann_json
    ${GSTREAMER_LIBS}
)

target_include_directories(PixelStream PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${GST_INCLUDE_DIRS}
)
