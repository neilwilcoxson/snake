cmake_minimum_required(VERSION 3.0)
project(snake)

# Linux:   apt install libsdl2-dev
#          (Note: check that the version is new enough if there are errors)
# Mac:     brew install sdl2
# Windows: set environment variable SDL2_DIR,
#          points to the directory containing sdl2-config.cmake
find_package(SDL2 2.0.18 REQUIRED)

add_executable(snake snake.cpp)
target_compile_features(snake PUBLIC cxx_std_17)
target_include_directories(snake PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(snake PRIVATE ${SDL2_LIBRARIES})

add_custom_target(format
    COMMAND clang-format -i ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.hpp
    COMMENT "Running clang-format")
add_custom_target(check-format
    COMMAND clang-format --dry-run ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.hpp
    COMMENT "Running clang-format --dry-run")