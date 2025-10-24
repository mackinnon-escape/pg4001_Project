#include "Menu.h"
#include "Input.h"
#include "Colours.h"

Menu::~Menu()
{
    for (auto item : items)
    {
        delete item;
    }
    items.clear();
    selectedItem = 0;
}

void Menu::AddItem(MenuItemCode code, const std::string& label)
{
    MenuItem* item = new MenuItem();
    item->code = code;
    item->label = label;
    items.push_back(item);
}

Menu::MenuItemCode Menu::GetMenuSelection(Input& input)
{
    if (items.empty()) return NONE;

    switch (input.GetKeyCode())
    {
    case SDLK_ESCAPE:
        return EXIT;
    case SDLK_UP:
        selectedItem = (selectedItem - 1 + static_cast<int>(items.size())) % static_cast<int>(items.size());
        break;
    case SDLK_DOWN:
        selectedItem = (selectedItem + 1) % static_cast<int>(items.size());
        break;
    case SDLK_KP_ENTER:
    case SDLK_RETURN:
        return items[selectedItem]->code;
    default:
        break;
    }

    return NONE;
}

void Menu::Render(tcod::Console& console)
{
    console.clear();

    // Try to load and draw background image
    static TCOD_Image* img = nullptr;
    if (!img)
    {
        img = TCOD_image_load("menu_background.png");
    }

    if (img)
    {
        int img_width, img_height;
        TCOD_image_get_size(img, &img_width, &img_height);
        TCOD_image_blit_2x(img, console.get(), 0, 0, 0, 0, img_width, img_height);
    }

    // Draw menu items
    int currentItem = 0;
    for (auto item : items)
    {
        TCODColor colour = (currentItem == selectedItem) ? LIGHTER_ORANGE : LIGHT_GREY;
        TCOD_ColorRGB tcod_colour = { colour.r, colour.g, colour.b };
        tcod::print(console, { 10, console.get_height() / 3 + currentItem * 3 },
            item->label, std::optional{ tcod_colour }, std::nullopt);
        currentItem++;
    }
}

/// ///////////////////////////////////////////////////

void GameMenu::Initialise(bool saveGameExists)
{
    AddItem(Menu::NEW_GAME, "New game");
    if (saveGameExists)
    {
        AddItem(Menu::CONTINUE, "Continue");
    }
    AddItem(Menu::EXIT, "Exit");
}