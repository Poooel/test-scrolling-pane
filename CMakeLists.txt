cmake_minimum_required(VERSION 3.12)
project(helloworld_with_helloimgui)
set(CMAKE_CXX_STANDARD 20)

# Build hello_imgui
# =================
add_subdirectory(externals/freetype)
add_subdirectory(externals/hello_imgui)

# Build your app
# ==============
hello_imgui_add_app(hello_world_ test-scrolling-pane/source/hello_world.main.cpp)
