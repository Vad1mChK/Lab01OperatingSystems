# test/CMakeLists.txt

# Define paths for test sources and includes
get_filename_component(MONOLITH_TEST_INCLUDE_PATH "." ABSOLUTE)
get_filename_component(MONOLITH_TEST_SOURCE_PATH "./${PROJECT_NAME}" ABSOLUTE)

# Set test target name
set(MONOLITH_TEST_TARGET "${PROJECT_NAME}-test")

# Gather all test source files
file(GLOB_RECURSE MONOLITH_TEST_SOURCES CONFIGURE_DEPENDS *.hpp *.cpp)

# Create the test executable
add_executable(${MONOLITH_TEST_TARGET} ${MONOLITH_TEST_SOURCES})

# Include directories for the test target
target_include_directories(${MONOLITH_TEST_TARGET} PUBLIC ${MONOLITH_TEST_INCLUDE_PATH})

# Link the test target with the main project library and Google Test libraries
target_link_libraries(
        ${MONOLITH_TEST_TARGET} PRIVATE
        ${PROJECT_NAME}
        GTest::gtest
        GTest::gmock
)

# Enable automatic discovery of Google Test tests
include(GoogleTest)
gtest_discover_tests(${MONOLITH_TEST_TARGET})
