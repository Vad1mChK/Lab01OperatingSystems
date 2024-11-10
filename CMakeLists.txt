cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(monolith VERSION 0.1.0)

option(MONOLITH_DEVELOPER "Enable ${PROJECT_NAME} developer mode" ON)
option(MONOLITH_BENCHMARK "Enable ${PROJECT_NAME} benchmark module" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(cmake/CompileOptions.cmake)

# Add main source directory
add_subdirectory(source)

# Google Test setup with FetchContent
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

# Add test directory only if developer mode is enabled
if (MONOLITH_DEVELOPER)
    add_subdirectory(test)
endif ()

# Add benchmark directory only if benchmark mode is enabled
if (MONOLITH_BENCHMARK)
    find_package(GBenchmark REQUIRED)
    add_subdirectory(benchmark)
endif ()
