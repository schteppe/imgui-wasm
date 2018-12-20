class Draw 
{
public:
    bool init();
    static void printErrors();
    static void shaderErrors(int shader);
    static void clear();
};

#define GL_CHECKED(command)\
    command;\
    for(int error = glGetError(); (error=glGetError()); error != GL_NO_ERROR)\
    {\
        emscripten_log(EM_LOG_CONSOLE|EM_LOG_C_STACK|EM_LOG_DEMANGLE, "glerror: %d", error);\
    }
