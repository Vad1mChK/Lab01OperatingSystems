get_filename_component(MONOLITH_INCLUDE_PATH "." ABSOLUTE)
get_filename_component(MONOLITH_SOURCE_PATH "./${PROJECT_NAME}" ABSOLUTE)

file(GLOB_RECURSE MONOLITH_SOURCES CONFIGURE_DEPENDS *.hpp *.cpp)

set(MONOLITH_LIB_SOURCES ${MONOLITH_SOURCES}
        monolith/shell/Shell.hpp
        monolith/shell/Shell.cpp
        monolith/shell/Command.hpp
        monolith/shell/Command.cpp
        monolith/shell/CommandFactory.hpp
        monolith/shell/CommandFactory.cpp
        monolith/shell/commands/HelpCommand.hpp
        monolith/shell/commands/HelpCommand.cpp
        monolith/shell/commands/ExitCommand.hpp
        monolith/shell/commands/ExitCommand.cpp
        monolith/shell/commands/HistoryCommand.hpp
        monolith/shell/commands/HistoryCommand.cpp
        monolith/shell/commands/PathToWorkingDirectoryCommand.hpp
        monolith/shell/commands/PathToWorkingDirectoryCommand.cpp
        monolith/shell/util/path_functions.hpp
        monolith/shell/util/path_functions.cpp
        monolith/shell/commands/ChangeDirectoryCommand.hpp
        monolith/shell/commands/ChangeDirectoryCommand.cpp
        monolith/shell/commands/ListFilesCommand.hpp
        monolith/shell/commands/ListFilesCommand.cpp
        monolith/shell/commands/RunCommand.hpp
        monolith/shell/commands/RunCommand.cpp
        monolith/shell/commands/RunNonBlockingCommand.hpp
        monolith/shell/commands/RunNonBlockingCommand.cpp
        monolith/shell/commands/RunCommand.cpp
        loaders/sleeper/Sleeper.hpp
        loaders/sleeper/Sleeper.cpp
        loaders/ema-sort-int/ExternalMemorySorter.hpp
        loaders/ema-sort-int/ExternalMemorySorter.cpp
        loaders/ema-sort-int/main.cpp
        loaders/arg-printer/main.cpp
        loaders/ram-sort-int/RamMemorySorter.hpp
        loaders/ram-sort-int/RamMemorySorter.cpp
        loaders/ram-sort-int/main.cpp
        loaders/estimate-csort/main.cpp
        common/unistd_check.hpp
        monolith/shell/util/run_functions.cpp
        monolith/shell/util/string_functions.hpp
        monolith/shell/util/string_functions.cpp
        monolith/shell/commands/ListProcessesCommand.hpp
        monolith/shell/commands/ListProcessesCommand.cpp
)

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
        loaders/ema-sort-int/ExternalMemorySorter.cpp
        loaders/ema-sort-int/ExternalMemorySorter.hpp
        loaders/ema-sort-int/main.cpp
        loaders/util/ema_ram_sorter_cli_constants.hpp
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


target_compile_options(ema-sort-int-opt PRIVATE -O3 -march=native -ffast-math -funroll-loops)

target_compile_options(ram-sort-int-opt PRIVATE -O3 -march=native -ffast-math -funroll-loops)

set(MONOLITH_APP_SOURCES monolith/app/Main.cpp)

add_library(${PROJECT_NAME} STATIC ${MONOLITH_LIB_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${MONOLITH_INCLUDE_PATH})

add_executable(${PROJECT_NAME}-app ${MONOLITH_APP_SOURCES})
set_property(TARGET ${PROJECT_NAME}-app PROPERTY OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(
        ${PROJECT_NAME}-app PRIVATE
        ${PROJECT_NAME}
)
