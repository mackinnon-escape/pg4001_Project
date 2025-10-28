#include "Gui.h"

#include <sstream>

#include "Actor.h"
#include "Colours.h"
#include "Engine.h"
#include "Destructible.h"
#include "Map.h"
#include "CustomEvents.h"
#include "Input.h"


static constexpr int PANEL_HEIGHT = 7;
static constexpr int BAR_WIDTH = 20;
static constexpr int MSG_X = BAR_WIDTH + 2;
static constexpr int MSG_HEIGHT = PANEL_HEIGHT - 1;

Gui::Gui(const Point pos, const int width, const int height, tcod::Console& main) : console(width, height), position(pos), guiWidth(width), guiHeight(height), mainConsole(main)
{
    SubscribeToEvents();
}

void Gui::SubscribeToEvents()
{
    EventManager::GetInstance()->Subscribe(EventType::HealthChanged,
        [&, this](const Event& e)
        {
            const auto& healthEvent = static_cast<const HealthChangedEvent&>(e);
            healthData.currentHp = healthEvent.currentHp;   
            healthData.maxHp = healthEvent.maxHp;   
        });

    EventManager::GetInstance()->Subscribe(EventType::MessageLogged,
        [&, this](const Event& e)
        {
            const auto& msgEvent = static_cast<const MessageEvent&>(e);
            AddMessage(msgEvent.message, msgEvent.col);
        });

    EventManager::GetInstance()->Subscribe(EventType::UpdateLevelAndXP,
        [&, this](const Event& e)
        {
            const auto& levelEvent = static_cast<const UpdateLevelAndXPEvent&>(e);
            xpData.level = levelEvent.level;
            xpData.currentXp = levelEvent.currentXp;
            xpData.xpForNextLevel = levelEvent.xpForNextLevel;
        });
    EventManager::GetInstance()->Subscribe(EventType::DungeonLevelChanged,
        [&, this](const Event& e)
        {
            const auto& dungeonEvent = static_cast<const DungeonLevelChangedEvent&>(e);
            dungeonLevel = dungeonEvent.newLevel;
        });
}


void Gui::Render(Input& input, ILocationProvider& locationProvider)
{
    console.clear();
        
    // draw the health bar
    RenderBar(1, 1, BAR_WIDTH, "HP", healthData.currentHp, healthData.maxHp, LIGHT_RED, DARKER_RED);

    // draw XP display showing level progress
    char xpText[64];
    snprintf(xpText, sizeof(xpText), "XP: %d/%d (Level %d)", xpData.currentXp, xpData.xpForNextLevel, xpData.level);
    tcod::print(console, { 1, 3 }, xpText, LIGHT_VIOLET, std::nullopt);

    // draw dungeon level
    char dungeonText[32];
    snprintf(dungeonText, sizeof(dungeonText), "Dungeon level: %d", dungeonLevel);
    tcod::print(console, { 1, 4 }, dungeonText, LIGHT_GREY, std::nullopt);

    RenderMessages();
    RenderMouseLook(input, locationProvider);

    // blit the GUI console on the root console
    tcod::blit(mainConsole, console, { position.x, position.y }, { 0, 0, guiWidth, guiHeight });
}

void Gui::RenderBar(int x, int y, int width, const std::string& name, int value, int maxValue, const TCODColor& barColor, const TCODColor& backColor)
{
    float percent = static_cast<float>(value) / static_cast<float>(maxValue);
    int filled = static_cast<int>(percent * static_cast<float>(width));
    for (int i = 0; i < width; ++i)
    {
        TCODColor color = (i < filled) ? barColor : backColor;
        console.at({ x + i, y }).bg = { color.r, color.g, color.b, 255 };
    }

    char label[64];
    (void)snprintf(label, sizeof(label), "%s : %d/%d", name.c_str(), value, maxValue);
    tcod::print(console, { x + width / 2, y }, label, WHITE, std::nullopt, TCOD_CENTER);
}

void Gui::RenderMouseLook(Input& input, ILocationProvider& locationProvider)
{
    Point mouseLocation{ input.GetMouseLocation() };
    if (!locationProvider.IsInFov(mouseLocation))
    {
        // if mouse is out of fov, nothing to render
        return;
    }

    std::string actorNames;
    bool first{ true };

    auto actors = locationProvider.GetActorsAt(mouseLocation);
    for (auto actor : actors)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            actorNames += ", ";
        }
        actorNames += actor->name;
    }

    // display the list of actors under the mouse cursor
    tcod::print(console, { 1, 0 }, actorNames, LIGHT_GREY, std::nullopt);
}

void Gui::AddMessage(const std::string& text, const TCODColor& col)
{
    if (text.empty()) return;
    // split our message into lines
    std::vector<std::string> lineList;
    std::string currentLine;
    std::stringstream ss(text);
    while (std::getline(ss, currentLine))
    {
        lineList.push_back(currentLine);
    }

    // for each line, add to the message log
    for (const auto& line : lineList)
    {
        // make room for the new message
        if (log.size() == MSG_HEIGHT)
        {
            Message* toRemove = log[0];
            log.erase(log.begin());
            delete toRemove;
        }

        Message* msg = new Message(line, col);
        log.push_back(msg);
    }
}

void Gui::RenderMessages()
{
    int y = 1;
    float colourCoefficient = 0.4f;
    for (auto message : log)
    {
        TCODColor scaled = message->col * colourCoefficient;
        TCOD_ColorRGB fg = { scaled.r, scaled.g, scaled.b };
        tcod::print(console, { MSG_X, y }, message->text, fg, std::nullopt);

        y++;
        if (colourCoefficient < 1.0f)
        {
            colourCoefficient += 0.3f;
        }
    }
}