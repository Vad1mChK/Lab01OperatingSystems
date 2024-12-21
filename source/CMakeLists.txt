# source/CMakeLists.txt

# Ensure that the parent CMakeLists.txt sets up the project and C++ standard
# Assuming that in the root CMakeLists.txt, you have:
# project(MonolithApp)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# Get absolute paths for include and source directories
get_filename_component(MONOLITH_INCLUDE_PATH "." ABSOLUTE)
get_filename_component(MONOLITH_SOURCE_PATH "./${PROJECT_NAME}" ABSOLUTE)

# Gather all .hpp and .cpp files recursively, but we'll exclude main.cpp files later
file(GLOB_RECURSE MONOLITH_SOURCES CONFIGURE_DEPENDS "*.hpp" "*.cpp")

# List of source files for the monolith shared library
# **Important:** Exclude any main.cpp files to prevent multiple definitions
set(MONOLITH_LIB_SOURCES
        # Shell components
        monolith/shell/Shell.hpp
        monolith/shell/Shell.cpp
        monolith/shell/Command.hpp
        monolith/shell/Command.cpp
        monolith/shell/CommandFactory.hpp
        monolith/shell/CommandFactory.cpp

        # Shell commands
        monolith/shell/commands/HelpCommand.hpp
        monolith/shell/commands/HelpCommand.cpp
        monolith/shell/commands/ExitCommand.hpp
        monolith/shell/commands/ExitCommand.cpp
        monolith/shell/commands/HistoryCommand.hpp
        monolith/shell/commands/HistoryCommand.cpp
        monolith/shell/commands/PathToWorkingDirectoryCommand.hpp
        monolith/shell/commands/PathToWorkingDirectoryCommand.cpp
        monolith/shell/commands/ChangeDirectoryCommand.hpp
        monolith/shell/commands/ChangeDirectoryCommand.cpp
        monolith/shell/commands/ListFilesCommand.hpp
        monolith/shell/commands/ListFilesCommand.cpp
        monolith/shell/commands/RunCommand.hpp
        monolith/shell/commands/RunCommand.cpp
        monolith/shell/commands/RunNonBlockingCommand.hpp
        monolith/shell/commands/RunNonBlockingCommand.cpp
        monolith/shell/commands/RunNonBlockingForCommand.hpp
        monolith/shell/commands/RunNonBlockingForCommand.cpp

        # Shell utilities
        monolith/shell/util/path_functions.hpp
        monolith/shell/util/path_functions.cpp
        monolith/shell/util/run_functions.cpp
        monolith/shell/util/string_functions.hpp
        monolith/shell/util/string_functions.cpp

        # Common utilities
        common/unistd_check.hpp

        # Loaders
        loaders/sleeper/Sleeper.hpp
        loaders/sleeper/Sleeper.cpp

        loaders/ema-sort-int/ExternalMemorySorter.hpp
        loaders/ema-sort-int/ExternalMemorySorter.cpp

        loaders/ema-ram-sort-int/UnifiedMemorySorter.hpp
        loaders/ema-ram-sort-int/UnifiedMemorySorter.cpp

#        loaders/ema-sort-int-directio/DirectIoExternalMemorySorter.hpp
#        loaders/ema-sort-int-directio/DirectIoExternalMemorySorter.cpp

        loaders/ram-sort-int/RamMemorySorter.hpp
        loaders/ram-sort-int/RamMemorySorter.cpp

        loaders/util/ema_ram_sorter_cli_constants.hpp
        loaders/util/sorter_utils.hpp
        loaders/util/sorter_utils.cpp
)

# Define executables that have their own main.cpp and do not contribute to the shared library
# These executables should **not** be part of any shared library
add_executable(sleeper
        loaders/sleeper/Sleeper.cpp
        loaders/sleeper/Sleeper.hpp
)

add_executable(arg-printer
        loaders/arg-printer/main.cpp
)

add_executable(ema-sort-int
        loaders/util/sorter_utils.cpp
        loaders/util/sorter_utils.hpp
        loaders/util/ema_ram_sorter_cli_constants.hpp
        loaders/ema-sort-int/ExternalMemorySorter.cpp
        loaders/ema-sort-int/ExternalMemorySorter.hpp
        loaders/ema-sort-int/main.cpp
)

add_executable(ema-sort-int-opt
        loaders/util/sorter_utils.cpp
        loaders/util/sorter_utils.hpp
        loaders/ema-sort-int/ExternalMemorySorter.cpp
        loaders/ema-sort-int/ExternalMemorySorter.hpp
        loaders/ema-sort-int/main.cpp
        loaders/util/ema_ram_sorter_cli_constants.hpp
)

add_executable(ram-sort-int
        loaders/util/sorter_utils.cpp
        loaders/util/sorter_utils.hpp
        loaders/ram-sort-int/RamMemorySorter.cpp
        loaders/ram-sort-int/RamMemorySorter.hpp
        loaders/ram-sort-int/main.cpp
        loaders/util/ema_ram_sorter_cli_constants.hpp
)

add_executable(ram-sort-int-opt
        loaders/util/sorter_utils.cpp
        loaders/util/sorter_utils.hpp
        loaders/ram-sort-int/RamMemorySorter.cpp
        loaders/ram-sort-int/RamMemorySorter.hpp
        loaders/ram-sort-int/main.cpp
        loaders/util/ema_ram_sorter_cli_constants.hpp
)

add_executable(estimate-csort
        loaders/estimate-csort/main.cpp
)

add_executable(ema-ram-sort-int
        loaders/util/sorter_utils.cpp
        loaders/util/sorter_utils.hpp
        loaders/util/ema_ram_sorter_cli_constants.hpp
        loaders/ema-ram-sort-int/main.cpp
        loaders/ema-ram-sort-int/UnifiedMemorySorter.cpp
        loaders/ema-ram-sort-int/UnifiedMemorySorter.hpp
        loaders/ema-sort-int/ExternalMemorySorter.hpp
        loaders/ema-sort-int/ExternalMemorySorter.cpp
        loaders/ram-sort-int/RamMemorySorter.cpp
        loaders/ram-sort-int/RamMemorySorter.hpp
)

# ----------------
# Linking Pre-Built lab2_library.so

# Path for lab2_library
set(LAB2_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/lib")
set(LAB2_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/lib")

# Find the pre-built shared library
find_library(LAB2_LIB lab2_library PATHS ${LAB2_LIBRARY_PATH} NO_DEFAULT_PATH)
if (NOT LAB2_LIB)
    message(FATAL_ERROR "lab2_library.so not found in ${LAB2_LIBRARY_PATH}")
endif()

# Find the include directory for lab2_library.hpp
find_path(LAB2_INCLUDE_DIR lab2_library.hpp PATHS ${LAB2_INCLUDE_PATH} NO_DEFAULT_PATH)
if (NOT LAB2_INCLUDE_DIR)
    message(FATAL_ERROR "lab2_library.hpp not found in ${LAB2_INCLUDE_PATH}")
endif()

# Informational messages for debugging
message(STATUS "Lab2 Library Path: ${LAB2_LIBRARY_PATH}")
message(STATUS "Lab2 Include Path: ${LAB2_INCLUDE_PATH}")
message(STATUS "Lab2 Lib Found: ${LAB2_LIB}")

# Define the `ema-sort-int-directio` executable that links against the pre-built lab2_library.so
add_executable(ema-sort-int-directio
        loaders/util/sorter_utils.cpp
        loaders/util/sorter_utils.hpp
        loaders/ema-sort-int-directio/main.cpp
        loaders/ema-sort-int-directio/DirectIoExternalMemorySorter.hpp
        loaders/ema-sort-int-directio/DirectIoExternalMemorySorter.cpp
        loaders/util/ema_ram_sorter_cli_constants.hpp
)

# Include the lab2_library headers
target_include_directories(ema-sort-int-directio PRIVATE ${LAB2_INCLUDE_DIR})

# Link against the pre-built lab2_library.so
target_link_libraries(ema-sort-int-directio PRIVATE ${LAB2_LIB})

# ----------------

# Compilation Options for Optimization Levels
target_compile_options(ema-sort-int PRIVATE -O0)
target_compile_options(ema-sort-int-opt PRIVATE -O3 -march=native -funroll-loops)
target_compile_options(ram-sort-int PRIVATE -O0)
target_compile_options(ram-sort-int-opt PRIVATE -O3 -march=native -funroll-loops)

# ----------------
# Define the monolith shared library without including any main.cpp files
add_library(${PROJECT_NAME} SHARED ${MONOLITH_LIB_SOURCES})

# Include directories for monolith shared library
target_include_directories(${PROJECT_NAME} PUBLIC ${MONOLITH_INCLUDE_PATH}
        PRIVATE ${LAB2_INCLUDE_PATH}
)

# Define the main application executable that links against the monolith shared library
add_executable(${PROJECT_NAME}-app
        monolith/app/Main.cpp
)

# Set the output name for the executable
set_property(TARGET ${PROJECT_NAME}-app PROPERTY OUTPUT_NAME ${PROJECT_NAME}-app)

# Link the monolith shared library with the application executable and the pre-built lab2_library.so
target_link_libraries(
        ${PROJECT_NAME}-app PRIVATE
        ${PROJECT_NAME}            # Link against the monolith shared library
        ${LAB2_LIB}                # Link against the pre-built lab2_library.so
)

# ----------------
