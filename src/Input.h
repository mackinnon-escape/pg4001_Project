#pragma once

#include <SDL3/SDL.h>

struct KeyEvent
{
    int scanCode;               // a code representing the place on the keyboard, regardless of what character is there
    unsigned int keyCode;       // a code that represents the character that was on the key pressed
    unsigned int modifiers;     // Modifier keys (Shift, Ctrl, Alt)
    char keyChar;

    void Clear() { scanCode = 0; keyCode = 0; keyChar = 0; modifiers = 0; }
};

class Input
{
public:
    bool CheckForEvent();
    int GetScanCode() const { return keyPressed.scanCode; };
    unsigned int GetKeyCode() const { return keyPressed.keyCode; };
    bool IsShiftKeyPressed() const 
    {
        return keyPressed.modifiers & SDL_KMOD_LSHIFT ||
            keyPressed.modifiers & SDL_KMOD_RSHIFT;
    };
    void ClearKey() { keyPressed.Clear(); }
private:
    KeyEvent keyPressed;
};