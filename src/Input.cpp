#include "Input.h"

#include "Input.h"
#include <cstdlib>

bool Input::CheckForEvent()
{
    SDL_Event event;
    SDL_WaitEvent(nullptr);  // Optional: sleep until events are available

    keyPressed.Clear();
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            std::exit(0);
        case SDL_EVENT_KEY_DOWN:
            keyPressed.scanCode = event.key.scancode;
            keyPressed.keyCode = static_cast<int>(event.key.key);
            keyPressed.keyChar = static_cast<char>(keyPressed.keyCode);
            keyPressed.modifiers = event.key.mod;
            break;
        default:
            break;
        }
    }
    return true;
}