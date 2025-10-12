#include "Gui.h"

#include <sstream>

#include "Actor.h"
#include "Colours.h"
#include "Engine.h"
#include "Destructible.h"
#include "Map.h"

static constexpr int PANEL_HEIGHT = 7;
static constexpr int BAR_WIDTH = 20;
static constexpr int MSG_X = BAR_WIDTH + 2;
static constexpr int MSG_HEIGHT = PANEL_HEIGHT - 1;

void Gui::Render()
{
    console.clear();

    // draw the health bar
    RenderBar(1, 1, BAR_WIDTH, "HP", static_cast<int>(Engine::GetInstance()->player->destructible->hp), static_cast<int>(Engine::GetInstance()->player->destructible->maxHp), LIGHT_RED, DARKER_RED);

    // draw the message log
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

    RenderMouseLook();

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


/// <summary>
/// message takes a colour and a formatted std::string
/// there are a variable number of arguments for this function
/// </summary>
/// <param name="col">colour of text</param>
/// <param name="format">formatted text to display in log</param>
/// <param name="...">formatting values to insert into formatted text</param>
void Gui::SendMessage(const TCODColor& col, const char* format, ...)
{
    // build the text from the format string and
    va_list vaArgs;
    char buf[128];
    va_start(vaArgs, format);
    (void)vsnprintf(buf, sizeof(buf), format, vaArgs);
    va_end(vaArgs);
    std::string messageContent{ buf };

    // split our message into lines
    std::vector<std::string> lineList;
    if (!messageContent.empty())
    {
        std::string currentLine;
        std::stringstream ss(messageContent);
        while (std::getline(ss, currentLine))
        {
            lineList.push_back(currentLine);
        }
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

void Gui::RenderMouseLook()
{
    Point mouseLocation{ Engine::GetInstance()->GetMouseLocation() };
    if (!Engine::GetInstance()->map->IsInFov(mouseLocation))
    {
        // if mouse is out of fov, nothing to render
        return;
    }

    std::string actorNames;
    bool first = true;
    for (auto actor : Engine::GetInstance()->actors)
    {
        // find actors under the mouse cursor
        if (actor->IsIn(mouseLocation))
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
    }

    // display the list of actors under the mouse cursor
    tcod::print(console, { 1, 0 }, actorNames, LIGHT_GREY, std::nullopt);
}