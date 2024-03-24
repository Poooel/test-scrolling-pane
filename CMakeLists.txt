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

# Includes
# ========
include_directories(test-scrolling-pane/include)

# Build your app
# ==============
FILE(GLOB_RECURSE SOURCES "test-scrolling-pane/*.cpp")
hello_imgui_add_app(test_scrolling_pane ${SOURCES})

# Extra options
# =============
if (EMSCRIPTEN)
    target_link_options(test_scrolling_pane PRIVATE -sPTHREAD_POOL_SIZE=5)
endif ()
