#include "Pickable.h"

#include <algorithm>
#include <vector>

#include "Actor.h"
#include "CustomEvents.h"
#include "Serialise.h"

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

bool Pickable::Use(Actor* usedItem, Actor* user, ILocationProvider& locationProvider) const
{
    std::vector<Actor*> targets;
    if (selector)
    {
        auto useCallback = [this](std::vector<Actor*>& targets, Actor* usedItem, Actor* user) -> bool { return this->Use(targets, usedItem, user); };
        selector->SelectTargets(targets, user, usedItem, locationProvider, useCallback);
    }
    else
    {
        targets.push_back(user); // self-target if no selector
    }

    if (targets.empty()) return false;

    return Use(targets, usedItem, user);
}

bool Pickable::Use(std::vector<Actor*>& targets, Actor* usedItem, Actor* user) const
{
    bool succeed = false;
    // Apply effect to all selected targets
    for (auto actor : targets)
    {
        if (effect->ApplyTo(actor))
        {
            succeed = true;
        }
    }

    // If the effect worked, consume the item
    if (succeed)
    {
        user->RemoveFromContainer(usedItem);
        delete usedItem;
    }

    return succeed;
}

void Pickable::Drop(Actor* droppedItem, Actor* dropper, std::vector<Actor*>& actors)
{
    if (dropper->RemoveFromContainer(droppedItem))
    {
        // Add back to the world
        actors.push_back(droppedItem);
        // Place at dropper's location
        droppedItem->SetLocation(dropper->GetLocation());
        EventManager::GetInstance()->Publish(MessageEvent(dropper->name + " drops a " + droppedItem->name + ".", LIGHT_GREY));
    }
}

void Pickable::Save(Saver& saver) const
{
    // Save whether we have a selector
    saver.PutInt(selector ? 1 : 0);
    if (selector)
    {
        selector->Save(saver);
    }

    // Save whether we have an effect
    saver.PutInt(effect ? 1 : 0);
    if (effect)
    {
        effect->Save(saver);
    }
}
void Pickable::Load(Loader& loader)
{
    bool hasSelector = loader.GetInt();
    if (hasSelector)
    {
        selector = std::make_unique<TargetSelector>(TargetSelector::SelectorType::NONE, -1);
        selector->Load(loader);
    }
    else
    {
        selector = nullptr;
    }

    bool hasEffect = loader.GetInt();
    if (hasEffect)
    {
        effect = Effect::Create(loader);
    }
    else
    {
        effect = nullptr;
    }
}