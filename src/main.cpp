#include "draw.h"
#include "funimgui.h"
#include <stdio.h>
#include <imgui.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

static Draw GDraw;

EMSCRIPTEN_KEEPALIVE
bool init()
{
    bool result = GDraw.init();
    FunImGui::init();
    return result;
}

EMSCRIPTEN_KEEPALIVE
void loop()
{
    FunImGui::BeginFrame();

    static bool bShowTestWindow = true;
    ImGui::ShowDemoWindow(&bShowTestWindow);
    Draw::clear();
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
