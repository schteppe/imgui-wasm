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
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

uint32_t shaderProgram;
int32_t attribLocationPosition;
int32_t g_colorUniformLocation;
uint32_t VBO, VAO;
std::array<float,9> vertices = {
    -0.5f, -0.5f, 0.0f, // left
    0.5f, -0.5f, 0.0f, // right
    0.0f,  0.5f, 0.0f  // top
};

bool g_done = false;
SDL_Window* g_window;
SDL_GLContext g_glcontext;

bool initTriangle()
{
    const char *vertexShaderSource = R"xxx(
        attribute vec3 aPos;
        void main()
        {
           gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )xxx";
    
    const char *fragmentShaderSource = R"xxx(
        uniform vec4 color;
        void main()
        {
           gl_FragColor = color;
        }
    )xxx";
    
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    {
        std::string shaderSource = vertexShaderSource;
    #ifdef __EMSCRIPTEN__
        shaderSource = "precision mediump float;\n" + shaderSource; // required in webgl
    #endif
        const char* tmp = shaderSource.c_str();
        glShaderSource(vertexShader, 1, &tmp, NULL);
    }
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    {
        std::string shaderSource = fragmentShaderSource;
#ifdef __EMSCRIPTEN__
        shaderSource = "precision mediump float;\n" + shaderSource; // required in webgl
#endif
        const char* tmp = shaderSource.c_str();
        glShaderSource(fragmentShader, 1, &tmp, NULL);
    }
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
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
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    g_colorUniformLocation = glGetUniformLocation(shaderProgram, "color");
    if (g_colorUniformLocation == -1){
        return false;
    }
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    
    return true;
}

void RenderTriangle(int x, int y, int width, int height, float time, glm::vec3 color, glm::vec3 bgcolor)
{
    glViewport(x, y, width, height);
    glClearColor(bgcolor.x, bgcolor.y, bgcolor.z, 1.0f);
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

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    glUseProgram(shaderProgram);
    glUniform4f(g_colorUniformLocation, color.x, color.y, color.z, 1.0f);
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
#ifdef __EMSCRIPTEN__
    ImGui::SameLine();
    if(ImGui::Button("View on Github"))
    {
        emscripten_run_script("window.location.href = 'https://github.com/schteppe/imgui-wasm';");
    }
#endif
    static glm::vec3 color = glm::vec3(0.7f, 0.3f, 0.2f);
    static glm::vec3 bgcolor = glm::vec3(0.2f);
    ImGui::ColorEdit3("Triangle Color", glm::value_ptr(color));
    ImGui::ColorEdit3("Background Color", glm::value_ptr(bgcolor));
    ImGui::End();

    //static bool g_show_test_window = true;
    //ImGui::ShowDemoWindow(&g_show_test_window);
    
    int w, h;
    SDL_GL_GetDrawableSize(g_window, &w, &h);
    RenderTriangle(0, 0, w, h, SDL_GetTicks() / 1000.0f, color, bgcolor);
    
    glViewport(0, 0, w, h);
    ImGui::Render();
    
    SDL_GL_SwapWindow(g_window);
}

bool initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Error: %s\n" << SDL_GetError() << '\n';
        return false;
    }
    
    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    g_window = SDL_CreateWindow(
        "ImGUI / WASM / WebGL demo", // title
        SDL_WINDOWPOS_CENTERED, // x
        SDL_WINDOWPOS_CENTERED, // y
        1280, 720, // width, height
        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI // flags
    );
    g_glcontext = SDL_GL_CreateContext(g_window);
    
    ImGui_ImplSdl_Init(g_window);
    
    return true;
}

void destroySDL()
{
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(g_glcontext);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}

void runMainLoop()
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (!g_done)
    {
        main_loop();
    }
#endif
}

int main(int, char**)
{
    if(!initSDL())
    {
        return EXIT_FAILURE;
    }
    
    if(!initTriangle())
    {
        destroySDL();
        return EXIT_FAILURE;
    }

    runMainLoop();
        
    destroySDL();

    return EXIT_SUCCESS;
}
