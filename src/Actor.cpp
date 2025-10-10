#include "Actor.h"

#include "Engine.h"
#include "Map.h"

void Actor::Render() const
{
    Engine::GetInstance()->console.at(position.x, position.y).ch = ch;
    Engine::GetInstance()->console.at(position.x, position.y).fg = TCOD_ColorRGBA{ colour.r, colour.g, colour.b, 255 };
}

bool Actor::Move(const Point& target)
{
    if (Engine::GetInstance()->map->IsWall(target))
    {
        return false;
    }

    for (auto actor : Engine::GetInstance()->actors)
    {
        if (actor->IsIn(target))
        {
            return false;    // Another actor is already there
        }
    }

    SetLocation(target);
    return true;
}