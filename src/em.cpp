#ifdef __EMSCRIPTEN__
#include "main.h"
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
EMSCRIPTEN_KEEPALIVE
int main() {
    bool bInitialized = init();
    if( bInitialized )
    {
        emscripten_set_main_loop(loop, 0, 1);
    }
}
#endif // #ifdef __EMSCRIPTEN__
