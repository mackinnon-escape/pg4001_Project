#include "Popup.h"

#include "SDL3/SDL.h"
#include "libtcod.h"

#include "Colours.h"
#include "Actor.h"
#include "Container.h"
#include "Engine.h"

void Popup::Update()
{
    if (int keyScanCode = static_cast<int>(inputHandler.GetKeyCode()))
    {
        if (keyScanCode == SDLK_ESCAPE)
        {
            done = true;
            inputHandler.ClearKey();
            return;
        }
    }
}

void Popup::Render(tcod::Console&)
{
    console.clear();

    // Draw border using box-drawing characters
    constexpr std::array<int, 9> BORDER =
    {
        9556, 9552, 9559,  // ???
        9553, ' ',  9553,  // ? ?
        9562, 9552, 9565,  // ???
    };

    tcod::draw_frame(console, { 0, 0, LEVELUP_WIDTH, LEVELUP_HEIGHT }, BORDER, foreground, std::nullopt);
}

void InventoryPopup::Update()
{
    Popup::Update();
    if (int keyScanCode = static_cast<int>(inputHandler.GetKeyCode()))
    {
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

    Popup::Render(mainConsole);

    tcod::ColorRGB fg = tcod::ColorRGB(200, 180, 50);
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

void LevelUpPopup::Update()
{
    if (int keyScanCode = static_cast<int>(inputHandler.GetKeyCode()))
    {
        inputHandler.ClearKey();
        if (keyScanCode == 'a' || keyScanCode == 'A')
        {
            player->destructible->ApplyLevelBoost(player, LevelBoost::CONSTITUTION);
            done = true;
        }
        else if (keyScanCode == 'b' || keyScanCode == 'B')
        {
            player->destructible->ApplyLevelBoost(player, LevelBoost::STRENGTH);
            done = true;
        }
        else if (keyScanCode == 'c' || keyScanCode == 'C')
        {
            player->destructible->ApplyLevelBoost(player, LevelBoost::AGILITY);
            done = true;
        }
    }
}

void LevelUpPopup::Render(tcod::Console& mainConsole)
{
    if (done) return;

    foreground = WHITE;
    Popup::Render(mainConsole);
    // Title
    std::string title = "Level " + std::to_string(level) + "!";
    tcod::print(console, { LEVELUP_WIDTH / 2, 2 }, title, YELLOW, std::nullopt, TCOD_CENTER);
    tcod::print(console, { LEVELUP_WIDTH / 2, 3 }, "Choose your improvement:", WHITE, std::nullopt, TCOD_CENTER);
    // Options
    tcod::print(console, { 2, 6 }, "a) Constitution (+20 HP)", WHITE, std::nullopt);
    tcod::print(console, { 2, 8 }, "b) Strength (+1 attack)", WHITE, std::nullopt);
    tcod::print(console, { 2, 10 }, "c) Agility (+1 defense)", WHITE, std::nullopt);

    // Center the popup on screen
    tcod::blit(mainConsole, console,
        { (mainConsole.get_width() - LEVELUP_WIDTH) / 2, (mainConsole.get_height() - LEVELUP_HEIGHT) / 2 },
        { 0, 0, console.get_width(), console.get_height() });
}

