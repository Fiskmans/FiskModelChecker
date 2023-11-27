cmake_minimum_required(VERSION 3.8.0)

project(fisk_model_checker)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

Include(FetchContent)

FetchContent_Declare(
  fisk_input
  GIT_REPOSITORY https://github.com/Fiskmans/fisk_input.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(fisk_input)

add_subdirectory(imgui)
add_subdirectory(src)