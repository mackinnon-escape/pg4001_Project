#ifndef GUI_H
#define GUI_H

#include <string>
#include <vector>
#include "libtcod.h"
#include "Point.h"

class Gui
{
protected:
    struct Message
    {
        Message(const std::string& text, const TCODColor& col) : col(col), text(text) {}
        std::string text;
        TCODColor col;
    };

public:
    Gui(const Point pos, const int width, const int height, tcod::Console& main) : console(width, height), position(pos), guiWidth(width), guiHeight(height), mainConsole(main) {}
    virtual ~Gui() = default;

    void Render();
    void SendMessage(const TCODColor& col, const char* text, ...);
    void Clear();
protected:
    void RenderBar(int x, int y, int width, const std::string& name, int value, int maxValue, const TCODColor& barColor, const TCODColor& backColor);
    void RenderMouseLook();

    tcod::Console console;
    tcod::Console& mainConsole;
    std::vector<Message*> log;

    int guiHeight{ 0 };
    int guiWidth{ 0 };
    Point position{ 0,0 };
};

#endif // GUI_H