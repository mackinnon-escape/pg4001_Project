#ifndef GUI_H
#define GUI_H

#include <string>
#include <vector>
#include "libtcod.h"
#include "Point.h"
#include "ILocationProvider.h"

class Input;

class Gui
{
protected:
    struct HealthBarData
    {
        int currentHp{ 0 };
        int maxHp{ 0 };
    } healthData;

    struct Message
    {
        Message(const std::string& text, const TCODColor& col) : col(col), text(text) {}
        std::string text;
        TCODColor col;
    };

    struct XPBarData
    {
        int currentXp{ 0 };
        int level{ 1 };
        int xpForNextLevel{ 200 };  // Initial value, will be updated via events
    } xpData;

public:
    Gui(const Point pos, const int width, const int height, tcod::Console& main);
    virtual ~Gui() = default;

    void Render(Input& input, ILocationProvider& locationProvider);
    void Clear();
protected:
    void RenderBar(int x, int y, int width, const std::string& name, int value, int maxValue, const TCODColor& barColor, const TCODColor& backColor);
    void RenderMouseLook(Input& input, ILocationProvider& locationProvider);
    void SubscribeToEvents();
    void RenderMessages();
    void AddMessage(const std::string& text, const TCODColor& col);

    tcod::Console console;
    tcod::Console& mainConsole;
    std::vector<Message*> log;

    int guiHeight{ 0 };
    int guiWidth{ 0 };
    Point position{ 0,0 };
    int dungeonLevel{ 1 };
};

#endif // GUI_H