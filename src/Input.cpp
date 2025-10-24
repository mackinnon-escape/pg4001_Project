#include "Input.h"

#include <cstdlib>

bool Input::CheckForEvent(tcod::Context& context)
{
    SDL_Event event;
    SDL_WaitEvent(nullptr);  // Optional: sleep until events are available

    keyPressed.Clear();
    mouse.Clear();

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
        case SDL_EVENT_MOUSE_BUTTON_UP:  // new case
        case SDL_EVENT_MOUSE_MOTION:     // new case
            CacheMouseValues(event, context);
            break;
        default:
            break;
        }
    }
    return true;
}

void Input::CacheMouseValues(SDL_Event& event, tcod::Context& context)
{
    TCOD_mouse_t tcod_mouse;
    TCOD_sys_process_mouse_event(&event, &tcod_mouse);
    context.convert_event_coordinates(event);

    switch (event.type)
    {
    case SDL_EVENT_MOUSE_MOTION:
        mouse.position = Point{ static_cast<int>(event.motion.x), static_cast<int>(event.motion.y) };
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        mouse.position = Point{ static_cast<int>(event.button.x), static_cast<int>(event.button.y) };
        mouse.isLeftMousePressed = event.button.button == SDL_BUTTON_LEFT;
        mouse.isRightMousePressed = event.button.button == SDL_BUTTON_RIGHT;
        break;
    default:
        break;
    }
}