cmake_minimum_required(VERSION 3.11)
include(FetchContent)


project(ImguiPlayground)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Imgui
FetchContent_Declare(imgui_external
	URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.8.tar.gz
	EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui_external)

# Create ImGui library from source files
add_library(imgui
    ${imgui_external_SOURCE_DIR}/imgui.cpp
    ${imgui_external_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_external_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_external_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_external_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_external_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp

)

# Include directories for ImGui
target_include_directories(imgui PUBLIC
    ${imgui_external_SOURCE_DIR}
    ${imgui_external_SOURCE_DIR}/backends
)

# Add the source files
add_executable(playground src/main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_SDL=2 -sUSE_WEBGL2=1 -sFULL_ES3=1 -sALLOW_MEMORY_GROWTH=1")

target_compile_options(playground PRIVATE
    "-sUSE_WEBGL2=1"
    "-sFULL_ES3=1"
)

target_link_options(playground PRIVATE
    "-sUSE_WEBGL2=1"
    "-sFULL_ES3=1"
)

# Link ImGui to the project
target_link_libraries(playground imgui)

# Set Emscripten output settings (can be changed if necessary)
set_target_properties(playground PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist
    SUFFIX ".html"
)
