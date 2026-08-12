include(FetchContent)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.14
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3)

# Dear ImGui ships no CMakeLists, so this only downloads it and we build it here.
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.9b
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(dalnim_imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
target_include_directories(dalnim_imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(dalnim_imgui PUBLIC SDL3::SDL3-static)

add_library(dalnim_render STATIC
    app.cpp
    array_view.cpp
    grid_view.cpp
    interval_view.cpp
    stack_view.cpp
    tree_view.cpp
    list_view.cpp
    graph_view.cpp
    dp_view.cpp
    camera.cpp
    stage.cpp
)
target_link_libraries(dalnim_render PUBLIC dalnim_core PRIVATE dalnim_imgui)
