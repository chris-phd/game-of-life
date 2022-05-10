cmake_minimum_required(VERSION 3.1...3.20)

set(C_STANDARD 11)
project(game_of_life C)

find_package(glfw3 3.3 REQUIRED)

include_directories(include)

add_executable(game_of_life 
               main.c 
               glad.c
               fileio.c
               time_control.c
               renderer.c
               window.c
               world.c
               matrix.c
               )

target_link_libraries(game_of_life
                      ${CMAKE_DL_LIBS}
                      glfw
                      m
                      GL)

add_executable(test_world
               test_world.c
               world.c
               time_control.c
               fileio.c
               )

target_link_libraries(test_world
                      m)

add_executable(test_matrix
               test_matrix.c
               matrix.c
               )

target_link_libraries(test_matrix
                      m)
