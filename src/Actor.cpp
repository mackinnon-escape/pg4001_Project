#include "Actor.h"

#include "Engine.h"
#include "Map.h"

Actor::~Actor()
{
    delete attacker;
    delete destructible;
    delete pickable;    // Add this line
    delete container;   // Add this line
}

void Actor::Update(Input& input, ILocationProvider& locationProvider)
{
    hasActedThisFrame = false;
    if (ai) 
    { 
        ai->Update(this, locationProvider, input);
    }
}

void Actor::Render(tcod::Console& console) const
{
    console.at(position.x, position.y).ch = ch;
    console.at(position.x, position.y).fg = TCOD_ColorRGBA{ colour.r, colour.g, colour.b, 255 };
}

/*
* transform the actor into a corpse!
*/
void Actor::ChangeToCorpse(std::string corpseName)
{
    ch = '%';
    colour = DARK_RED;
    name = corpseName;
    blocks = false;
}

int Actor::TakeDamage(int damage)
{
    if (destructible)
    {
        return destructible->TakeDamage(this, damage);
    }
    return 0;
}

bool Actor::RemoveFromContainer(Actor* item)
{
    if (container)
    {
        container->Remove(item);
        return true;
    }
    return false;
}