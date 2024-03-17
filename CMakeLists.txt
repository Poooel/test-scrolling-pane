cmake_minimum_required(VERSION 3.21)
project(test_scrolling_pane)
set(CMAKE_CXX_STANDARD 20)

# Options
# =======
option(HELLOIMGUI_USE_FREETYPE "Use freetype for text rendering" OFF)

if (EMSCRIPTEN)
    option(HELLOIMGUI_EMSCRIPTEN_PTHREAD "Build emscripten with multithreading support" ON)
endif ()

# Build hello_imgui
# =================
add_subdirectory(externals/hello_imgui)

# Build your app
# ==============
hello_imgui_add_app(test_scrolling_pane test-scrolling-pane/source/main.cpp)

# Extra options
# =============
if (EMSCRIPTEN)
    target_link_options(test_scrolling_pane PRIVATE -sPTHREAD_POOL_SIZE=5)
endif()
