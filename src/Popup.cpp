#include "Popup.h"

#include "SDL3/SDL.h"
#include "libtcod.h"

#include "Colours.h"
#include "Actor.h"
#include "Container.h"
#include "Engine.h"

void InventoryPopup::Update()
{
    if (int keyScanCode = static_cast<int>(inputHandler.GetKeyCode()))
    {
        if (keyScanCode == SDLK_ESCAPE)
        {
            inputHandler.ClearKey();
            done = true;
            return;
        }

        // Check for letter keys (a-z)
        if (keyScanCode >= 'a' && keyScanCode <= 'z')
        {
            int index = keyScanCode - 'a';
            if (index < static_cast<int>(owner->container->inventory.size()))
            {
                done = true;
                Actor* item = owner->container->inventory[index];
                if (action && item)
                {
                    action(item, owner);
                }
            }
        }
    }
}

void InventoryPopup::Render(tcod::Console& mainConsole)
{
    if (done) return;

    console.clear();
    tcod::ColorRGB fg = tcod::ColorRGB(200, 180, 50);

    // Draw border using box-drawing characters
    constexpr std::array<int, 9> BORDER =
    {
        9556, 9552, 9559,
        9553, ' ',  9553,
        9562, 9552, 9565,
    };
    tcod::draw_frame(console, { 0, 0, console.get_width(), console.get_height() }, BORDER, fg, std::nullopt);
    tcod::print(console, { 2, 0 }, "inventory", fg, std::nullopt);

    // List items with keyboard shortcuts
    int shortcut = 'a';
    int y = 2;
    for (auto& actor : owner->container->inventory)
    {
        char itemName[128];
        int printed = snprintf(itemName, sizeof(itemName), "(%c) %s", shortcut++, actor->name.c_str());
        if (printed <= static_cast<int>(sizeof(itemName)))
        {
            tcod::print(console, { 2, y++ }, itemName, WHITE, std::nullopt);
        }
    }

    tcod::blit(mainConsole, console, { (mainConsole.get_width() - INVENTORY_WIDTH) / 2, (mainConsole.get_height() - INVENTORY_HEIGHT) / 2 },
        { 0, 0, console.get_width(), console.get_height() });
}