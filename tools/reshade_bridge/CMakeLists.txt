cmake_minimum_required(VERSION 3.12.1)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(fsr2_reshade_bridge LANGUAGES CXX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

set(RESHADE_INCLUDE_DIR "" CACHE PATH "Path to ReShade include directory containing reshade.hpp")

if(NOT RESHADE_INCLUDE_DIR)
    message(WARNING "RESHADE_INCLUDE_DIR is not set. fsr2_reshade_bridge will not be built.")
    return()
endif()

add_library(fsr2_reshade_bridge SHARED
    fsr2_reshade_bridge.cpp)

target_compile_features(fsr2_reshade_bridge PRIVATE cxx_std_17)
target_compile_definitions(fsr2_reshade_bridge PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
target_include_directories(fsr2_reshade_bridge PRIVATE "${RESHADE_INCLUDE_DIR}")

set_target_properties(fsr2_reshade_bridge PROPERTIES
    OUTPUT_NAME "fsr2_reshade_bridge.addon64"
    PREFIX ""
    SUFFIX ""
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin")
