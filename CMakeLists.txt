cmake_minimum_required(VERSION 3.10)
project(lab2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Замеры имеют смысл только с оптимизацией: по умолчанию собираем Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Библиотека (логика решения ребусов)
add_library(rebus STATIC
    include/rebus.h
    src/rebus_internal.h
    src/rebus.c
    src/parser.c
    src/solver_linear.c
    src/solver_general.c
    src/format.c
)
target_include_directories(rebus
    PUBLIC include
    PRIVATE src
)
target_compile_options(rebus PRIVATE -Wall -Wextra)

# Консольная программа
add_executable(lab2 app/main.c)
target_link_libraries(lab2 PRIVATE rebus)
target_compile_options(lab2 PRIVATE -Wall -Wextra)

# Тесты
enable_testing()
add_executable(test_rebus tests/test_rebus.c)
target_link_libraries(test_rebus PRIVATE rebus)
target_compile_options(test_rebus PRIVATE -Wall -Wextra)
add_test(NAME rebus COMMAND test_rebus)

# Замер времени 
add_executable(bench bench/bench.c)
target_link_libraries(bench PRIVATE rebus)
target_compile_options(bench PRIVATE -Wall -Wextra)
