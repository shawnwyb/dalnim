# The app. Everything that touches a window or draws pixels lives here for now;
# it becomes its own `dalnim_render` library once SDL and ImGui arrive.
add_executable(dalnim main.cpp)
