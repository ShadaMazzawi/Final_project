cmake_minimum_required(VERSION 3.14)
project(perlin_noise)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# --- stb_image_write (single header, used for the headless PNG preview export) ---
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(stb)

add_executable(perlin_noise
    src/main.cpp
    src/perlin_noise.cpp
    src/perlin_noise.h
    src/texture_presets.cpp
    src/texture_presets.h
)
target_include_directories(perlin_noise PRIVATE src ${stb_SOURCE_DIR})