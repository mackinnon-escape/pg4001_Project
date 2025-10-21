#include "Container.h"
#include "Actor.h"

Container::~Container()
{
    for (auto actor : inventory)
    {
        delete actor;
    }
    inventory.clear();
}

bool Container::Add(Actor* actor)
{
    if (maxSize > 0 && static_cast<int>(inventory.size()) >= maxSize)
    {
        return false;     // inventory full
    }

    inventory.push_back(actor);
    return true;
}

void Container::Remove(Actor* actor)
{
    // here is the erase-remove idiom
    auto firstRemovedItem = std::remove(inventory.begin(), inventory.end(), actor);
    inventory.erase(firstRemovedItem, inventory.end());
}