#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>
#include "Engine.h"

int main()
{
    Engine engine;
    engine.Run();

    return 0;
}