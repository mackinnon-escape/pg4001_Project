#include "Actor.h"

#include "Engine.h"


void Actor::Render() const
{
    Engine::GetInstance()->console.at(position.x, position.y).ch = ch;
    Engine::GetInstance()->console.at(position.x, position.y).fg = TCOD_ColorRGBA{ colour.r, colour.g, colour.b, 255 };
}