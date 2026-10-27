cmake_minimum_required(VERSION 3.16)
project(mini-postgres C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(mini_postgres ${SOURCES})

# Tests
enable_testing()
add_executable(test_storage tests/test_page.c src/storage/page.c)
add_test(NAME TestStorage COMMAND test_storage)
