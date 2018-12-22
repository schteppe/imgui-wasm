cmake_minimum_required (VERSION 3.0)

project(imguiTest)

if(${EMSCRIPTEN})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/index.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
endif()

include_directories(src external ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})

add_executable(index
    external/imgui.cpp
    external/imgui_demo.cpp
    external/imgui_draw.cpp
    src/imgui_impl_sdl.cpp
    src/main.cpp
)

target_link_libraries(index ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})   
