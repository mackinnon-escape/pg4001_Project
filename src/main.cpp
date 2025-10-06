#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>
#include "Engine.h"

int main()
{
    Engine::GetInstance()->Run();
    return 0;
}