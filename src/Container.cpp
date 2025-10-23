#include "Container.h"
#include "Actor.h"
#include "Serialise.h"

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

void Container::Save(Saver& saver) const
{
    saver.PutInt(maxSize);
    saver.PutInt(static_cast<int>(inventory.size()));
    for (Actor* actor : inventory)
    {
        actor->Save(saver);
    }
}

void Container::Load(Loader& loader)
{
    maxSize = loader.GetInt();
    int inventorySize = loader.GetInt();
    for (int i = 0; i < inventorySize; i++)
    {
        Actor* actor = new Actor();
        actor->Load(loader);
        inventory.push_back(actor);
    }
}