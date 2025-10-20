#pragma once

#include <SDL3/SDL.h>
#include "libtcod.h"
#include "Point.h"

struct KeyEvent
{
    int scanCode;               // a code representing the place on the keyboard, regardless of what character is there
    unsigned int keyCode;       // a code that represents the character that was on the key pressed
    unsigned int modifiers;     // Modifier keys (Shift, Ctrl, Alt)
    char keyChar;

    void Clear() { scanCode = 0; keyCode = 0; keyChar = 0; modifiers = 0; }
};

struct MouseEvent
{
    Point position{};
    bool isLeftMousePressed{ false };
    bool isRightMousePressed{ false };
    void Clear() { isLeftMousePressed = isRightMousePressed = false; position = Point::Zero; }
};

class Input
{
public:
    bool CheckForEvent(tcod::Context& context);
    int GetScanCode() const { return keyPressed.scanCode; };
    unsigned int GetKeyCode() const { return keyPressed.keyCode; };
    bool IsShiftKeyPressed() const 
    {
        return keyPressed.modifiers & SDL_KMOD_LSHIFT ||
            keyPressed.modifiers & SDL_KMOD_RSHIFT;
    };
    void ClearKey() { keyPressed.Clear(); }

    void CacheMouseValues(SDL_Event& event, tcod::Context& context);
    Point GetMouseLocation() const { return mouse.position; }
    void ClearMouse() { mouse.Clear(); }

private:
    KeyEvent keyPressed{};
    MouseEvent mouse{ {0,0}, false, false };
};