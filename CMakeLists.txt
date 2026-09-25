cmake_minimum_required(VERSION 3.14)
project(perlin_noise)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
find_package(Threads REQUIRED)

# --- stb_image_write (single header, used for the headless PNG preview export) ---
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(stb)

# --- minifb (window + framebuffer) ---
set(MINIFB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MINIFB_BUILD_VERSION_INFO OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    minifb
    GIT_REPOSITORY https://github.com/emoon/minifb.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(minifb)

# --- microui (immediate-mode UI; header + single C file, no CMakeLists) ---
FetchContent_Declare(
    microui
    GIT_REPOSITORY https://github.com/rxi/microui.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(microui)

# microui_lib also carries atlas_data.c, a small C shim exposing microui's
# bundled default font/icon atlas (demo/atlas.inl uses C99 designated array
# initializers our C++ files can't include directly).
add_library(microui_lib STATIC
    ${microui_SOURCE_DIR}/src/microui.c
    src/atlas_data.c
    src/atlas_data.h
)
target_include_directories(microui_lib PUBLIC src ${microui_SOURCE_DIR}/src ${microui_SOURCE_DIR}/demo)

add_executable(perlin_noise
    src/main.cpp
    src/app.cpp
    src/app.h
    src/perlin_noise.cpp
    src/perlin_noise.h
    src/texture_presets.cpp
    src/texture_presets.h
    src/ui_bridge.h
    src/ui_renderer.cpp
    src/ui_renderer.h
)
target_include_directories(perlin_noise PRIVATE
    src
    ${stb_SOURCE_DIR}
    ${microui_SOURCE_DIR}/src
    ${minifb_SOURCE_DIR}/include
)
target_link_libraries(perlin_noise PRIVATE microui_lib minifb Threads::Threads)