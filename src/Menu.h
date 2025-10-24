#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>
#include "libtcod.h"

class Input;

class Menu
{
public:
    enum MenuItemCode
    {
        NONE,
        NEW_GAME,
        CONTINUE,
        EXIT
    };

    Menu() = default;
    virtual ~Menu();

    void Render(tcod::Console& console);
    MenuItemCode GetMenuSelection(Input& input);

protected:
    struct MenuItem
    {
        MenuItemCode code;
        std::string label;
    };

    void AddItem(MenuItemCode code, const std::string& label);

private:
    std::vector<MenuItem*> items{};
    int selectedItem{ 0 };
};

class GameMenu : public Menu
{
public:
    void Initialise(bool saveGameExists);
};

#endif // MENU_H