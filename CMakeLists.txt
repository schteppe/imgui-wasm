cmake_minimum_required (VERSION 3.0)

project(imguiTest)

include_directories(src external)

add_executable(index
    external/imgui.cpp
    external/imgui_demo.cpp
    external/imgui_draw.cpp
    src/em.cpp
    src/draw.cpp
    src/funimgui.cpp
    src/main.cpp
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shell.html
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
