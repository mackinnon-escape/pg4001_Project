#include "Pickable.h"

#include <algorithm>
#include <vector>

#include "Actor.h"

bool Pickable::Pick(Actor* pickedItem, Actor* newOwner, std::vector<Actor*>& actors)
{
    // put 'pickedItem' in the container
    if (newOwner->AddToContainer(pickedItem))
    {
        // if successfully added to inventory, remove from actors collection
        // std::remove shuffles elements to the back and returns iterator to them
        auto ownerIterator = std::remove(actors.begin(), actors.end(), pickedItem);
        // erase removes the elements from ownerIterator to end
        actors.erase(ownerIterator, actors.end());
        return true;
    }
    return false;
}

bool Pickable::Use(Actor* usedItem, Actor* user) const
{
    bool succeed = false;
    if (effect->ApplyTo(user))
    {
        succeed = true;
        user->RemoveFromContainer(usedItem);
        delete usedItem;
    }

    return succeed;
}