#include "draw.h"
#include "funimgui.h"
#include <stdio.h>
#include <imgui.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#include <iostream>
#include <math.h>

int shaderProgram;
unsigned int VBO, VAO;
float vertices[] = {
    -0.5f, -0.5f, 0.0f, // left  
    0.5f, -0.5f, 0.0f, // right 
    0.0f,  0.5f, 0.0f  // top   
}; 

void initTriangle(){

    const char *vertexShaderSource = ""
        "attribute vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char *fragmentShaderSource = "\n"
        "void main()\n"
        "{\n"
        "   gl_FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
        "}\n\0";

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

void RenderTriangle(float time){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    vertices[0] = sin(time);
    vertices[1] = cos(time);

    vertices[3] = sin(time + 2.0f * M_PI / 3.0f);
    vertices[4] = cos(time + 2.0f * M_PI / 3.0f);

    vertices[7] = cos(time + 4.0f * M_PI / 3.0f);
    vertices[6] = sin(time + 4.0f * M_PI / 3.0f);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // draw our first triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


static Draw GDraw;

EMSCRIPTEN_KEEPALIVE
bool init()
{
    bool result = GDraw.init();
    FunImGui::init();
    initTriangle();
    return result;
}

EMSCRIPTEN_KEEPALIVE
void loop()
{
    FunImGui::BeginFrame();

    static bool bShowTestWindow = true;
    ImGui::ShowDemoWindow(&bShowTestWindow);
    Draw::clear();
    

    ImGui::SetNextWindowPos(ImVec2(650,50), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_Once);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
    ImGui::Begin("WebGL content", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::BeginChild("glWindow");
    ImVec2 min = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    ImGui::EndChild();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    // Test
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = io.DisplaySize.x * io.DisplayFramebufferScale.x;
    int fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
    int x = (int)(min.x * io.DisplayFramebufferScale.x);
    int y = (int)(min.y * io.DisplayFramebufferScale.y);
    int w = (int)(size.x * io.DisplayFramebufferScale.x);
    int h = (int)(size.y * io.DisplayFramebufferScale.y);
    if(w>0 && h>0){
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, fb_height-y-h, w, h);
        glViewport(x, fb_height-y-h, w, h);
        //glClearColor(1.f,0.1f,0.1f,1);
        //glClear(GL_COLOR_BUFFER_BIT);
        RenderTriangle(ImGui::GetTime());
    }

    ImGui::Render();
}

EMSCRIPTEN_KEEPALIVE
int main()
{
    bool bInitialized = init();
    if( bInitialized )
    {
        emscripten_set_main_loop(loop, 0, 1);
    }
}
