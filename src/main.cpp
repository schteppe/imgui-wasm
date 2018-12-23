#include <imgui.h>
#include "imgui_impl_sdl.h"

#include <SDL.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArrays glBindVertexArraysAPPLE
#define glGenVertexArray glGenVertexArrayAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#else
#include <SDL_opengles2.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#endif

#include <math.h>
#include <iostream>
#include <glm/glm.hpp>

int shaderProgram;
int attribLocationPosition;
unsigned int VBO, VAO;
float vertices[] = {
    -0.5f, -0.5f, 0.0f, // left
    0.5f, -0.5f, 0.0f, // right
    0.0f,  0.5f, 0.0f  // top
};

bool g_done = false;
SDL_Window* g_window;
bool g_show_test_window = true;
ImVec4 g_clear_color = ImColor(114, 144, 154);

void initTriangle()
{
    const char *vertexShaderSource = R"xxx(
        attribute vec3 aPos;
        void main()
        {
           gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )xxx";
    
    const char *fragmentShaderSource = R"xxx(
        void main()
        {
           gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )xxx";
    
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    attribLocationPosition = glGetAttribLocation(shaderProgram, "aPos");
    
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

void RenderTriangle(int x, int y, int width, int height, float time)
{
    glViewport(x, y, width, height);
    glClearColor(g_clear_color.x, g_clear_color.y, g_clear_color.z, g_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    
    vertices[0] = sin(time);
    vertices[1] = cos(time);
    
    vertices[3] = sin(time + 2.0f * M_PI / 3.0f);
    vertices[4] = cos(time + 2.0f * M_PI / 3.0f);
    
    vertices[7] = cos(time + 4.0f * M_PI / 3.0f);
    vertices[6] = sin(time + 4.0f * M_PI / 3.0f);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(attribLocationPosition);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void main_loop()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSdl_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            g_done = true;
    }
    
    ImGui_ImplSdl_NewFrame(g_window);
    
    ImGui::Begin("Demo");
    ImGui::Text("Just a WebAssembly demo.");
    ImGui::End();

    //ImGui::ShowDemoWindow(&g_show_test_window);
    
    int w, h;
    SDL_GL_GetDrawableSize(g_window, &w, &h);
    RenderTriangle(0, 0, w, h, SDL_GetTicks() / 1000.0f);
    
    glViewport(0, 0, w, h);
    ImGui::Render();
    
    SDL_GL_SwapWindow(g_window);
}

int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    g_window = SDL_CreateWindow("ImGui SDL2+OpenGLES+Emscripten example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_GLContext glcontext = SDL_GL_CreateContext(g_window);
    
    ImGui_ImplSdl_Init(g_window);
    
    initTriangle();
    
    // Main loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (!g_done)
    {
        main_loop();
    }
#endif
    
    // Cleanup
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(g_window);
    SDL_Quit();

    return 0;
}
