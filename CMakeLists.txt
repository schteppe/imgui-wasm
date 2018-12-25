cmake_minimum_required (VERSION 3.0)

project(imguiTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wno-missing-braces")

if(${EMSCRIPTEN})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -s USE_SDL=2 -std=c++1z")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/index.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
endif()

include_directories(src)
include_directories(SYSTEM external ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})

add_executable(index
    external/imgui.h
    external/imgui.cpp
    external/imgui_demo.cpp
    external/imgui_draw.cpp
    src/imgui_impl_sdl.h
    src/imgui_impl_sdl.cpp
    src/main.cpp
)

source_group(external REGULAR_EXPRESSION external)
source_group(src REGULAR_EXPRESSION src)

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")

target_link_libraries(index ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})   
