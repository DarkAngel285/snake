cmake_minimum_required(VERSION 3.31.3)
project(snake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/doubly_linked_list.c
    ${PROJECT_SOURCE_DIR}/src/termios_init.c ${PROJECT_SOURCE_DIR}/src/game.c)
add_executable(snake ${SOURCES})
target_include_directories(snake PRIVATE ${PROJECT_SOURCE_DIR}/include)
