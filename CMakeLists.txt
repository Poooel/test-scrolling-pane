cmake_minimum_required(VERSION 3.12)
project(test_scrolling_pane)
set(CMAKE_CXX_STANDARD 20)

# Build hello_imgui
# =================
add_subdirectory(externals/freetype)
add_subdirectory(externals/hello_imgui)

# Build your app
# ==============
hello_imgui_add_app(test_scrolling_pane test-scrolling-pane/source/main.cpp)
