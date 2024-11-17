cmake_minimum_required(VERSION 3.28)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

project(monolith VERSION 0.1.0)

option(MONOLITH_BENCHMARK "Enable ${PROJECT_NAME} benchmark module" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(cmake/CompileOptions.cmake)

# Add main source directory
add_subdirectory(source)
add_subdirectory(test)

# Google Test setup with FetchContent
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

# Add benchmark directory only if benchmark mode is enabled
if (MONOLITH_BENCHMARK)
    find_package(GBenchmark REQUIRED)
    add_subdirectory(benchmark)
endif ()
