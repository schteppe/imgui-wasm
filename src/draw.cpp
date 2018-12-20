#include "draw.h"
#include <stdio.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

const int LOGFLAGS =  EM_LOG_CONSOLE|EM_LOG_C_STACK|EM_LOG_DEMANGLE;

bool Draw::init()
{
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
 
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context(nullptr, &attrs);
    if(context > 0)
    {
        if(emscripten_webgl_make_context_current(context) == EMSCRIPTEN_RESULT_SUCCESS)
        {
            EmscriptenFullscreenStrategy strategy;
            strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT;
            strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF; // EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
            strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
            return emscripten_enter_soft_fullscreen("canvas", &strategy) == EMSCRIPTEN_RESULT_SUCCESS;
        }
    }
    EM_ASM("document.body.innerText = 'Your browser does not support WebGL.';");
    return false;
}

void Draw::printErrors()
{
    int error = GL_NO_ERROR;
    while((error = glGetError()) != GL_NO_ERROR)
    {
        emscripten_log(LOGFLAGS, "glerror: %d", error);
    }
}

void Draw::shaderErrors(int shader)
{
    int ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if(!ok) {
        printf("Shader error\n");
        int infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char* infoLog = new char[infoLen+1];
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            printf("Shader failed:\n%s\n ", infoLog);
            delete[] infoLog;
        }
    }
}

void Draw::clear()
{
    glClearColor(0.1f,0.1f,0.1f,1);
    glClear(GL_COLOR_BUFFER_BIT);
}
