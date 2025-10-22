#include "Ai.h"

#include "Actor.h"
#include "Engine.h"
#include "Map.h"
#include "Point.h"
#include "Gui.h"
#include "Colours.h"
#include "Input.h"
#include "CustomEvents.h"
#include "Popup.h"

void PlayerAi::Update(Actor* owner, ILocationProvider& locationProvider, Input& input)
{
    if (owner->IsDead())  return;
    owner->hasActedThisFrame = false;
    
    int dx{ 0 };
    int dy{ 0 };
    switch (unsigned int key = input.GetKeyCode())
    {
    case SDLK_UP: dy = -1; break;
    case SDLK_DOWN: dy = 1; break;
    case SDLK_LEFT: dx = -1; break;
    case SDLK_RIGHT: dx = 1; break;
    default:
        if (key > 0 && key <= 0x00ff)    // ASCII range
        {
            bool playerActed = HandleActionKey(owner, key, locationProvider, input);
            if (playerActed)
            {
                owner->hasActedThisFrame = true;
            }
        }
        break;
    }
    
    if (dx != 0 || dy != 0)
    {
        Point dp{ dx, dy };
        owner->hasActedThisFrame = true;
        MoveOrAttack(owner, owner->GetLocation() + dp, locationProvider);
    }
}

bool PlayerAi::MoveOrAttack(Actor* owner, const Point& target, ILocationProvider& locationProvider)
{
    if (locationProvider.IsWall(target)) return false;
    // look for living actors to attack

    std::vector<Actor*> actorsInTarget = locationProvider.GetActorsAt(target);
    for (auto actor : actorsInTarget)
    {
        if (actor->IsAlive())
        {
            owner->Attack(actor);
            return false;
        }
    }

    // look for corpses or items
    for (auto actor : locationProvider.GetActors())
    {
        if ((actor->IsDead() || actor->pickable != nullptr) && actor->IsIn(target))
        {
            EventManager::GetInstance()->Publish(MessageEvent("There's a " + actor->name + " here", LIGHT_GREY));
        }
    }

    owner->SetLocation(target);
    return true;
}

bool PlayerAi::HandleActionKey(Actor* owner, unsigned int ascii, ILocationProvider& locationProvider, Input& input)
{
    switch (ascii)
    {
    case 'g': // get/pickup item
    {
        bool found = false;
        for (auto actor : locationProvider.GetActorsAt(owner->GetLocation()))
        {
            if (actor->pickable)
            {
                found = true;
                if (Pickable::Pick(actor, owner, locationProvider.GetActors()))
                {
                    EventManager::GetInstance()->Publish(MessageEvent("You pick up the " + actor->name + ".", LIGHT_GREY));
                }
                else
                {
                    EventManager::GetInstance()->Publish(MessageEvent("Your inventory is full.", DARK_RED));
                }
                break;
            }
        }
        if (!found)
        {
            EventManager::GetInstance()->Publish(MessageEvent("There's nothing here that you can pick up.", LIGHT_GREY));
        }

        return true;
    }
    break;

    case 'i': // display inventory
    {
        auto inventoryPopup = new InventoryPopup(owner,
            [&](Actor* item, Actor* owner) -> void
            {
                if (item->pickable)
                {
                    item->pickable->Use(item, owner);
                }
            }, input);
        EventManager::GetInstance()->Publish(PopupLaunchedEvent(inventoryPopup));
    }
    break;
    case 'd': // drop item
    {
        auto dropPopup = new InventoryPopup(owner,
            [&](Actor* item, Actor* owner) -> void
            {
                if (item && item->pickable && owner)
                {
                    Pickable::Drop(item, owner, locationProvider.GetActors());
                }
            }, input);
        EventManager::GetInstance()->Publish(PopupLaunchedEvent(dropPopup));
        return true;
    }
    }
    return false;
}


// how many turns the monster chases the player
// after losing his sight
static constexpr int TRACKING_TURNS{ 3 };

void MonsterAi::Update(Actor* owner, ILocationProvider& locationProvider)
{
    if (owner->IsDead()) return;

    if (locationProvider.IsInFov(owner->GetLocation()))
    {
        // we can see the player. move towards him
        moveCount = TRACKING_TURNS;
    }
    else
    {
        moveCount--;
    }

    if (moveCount > 0)
    {
        MoveOrAttack(owner, locationProvider.GetPlayerLocation(), locationProvider);
    }
}

void MonsterAi::MoveOrAttack(Actor* owner, const Point& target, ILocationProvider& locationProvider)
{
    int dx{ target.x - owner->GetLocation().x };
    int dy{ target.y - owner->GetLocation().y };
    float distance{ sqrtf(static_cast<float>(dx * dx + dy * dy)) };
    if (distance >= 2)
    {
        dx = static_cast<int>(round(static_cast<float>(dx) / distance));
        dy = static_cast<int>(round(static_cast<float>(dy) / distance));
        int stepDx{ dx > 0 ? 1 : -1 };
        int stepDy{ dy > 0 ? 1 : -1 };
        Point dp{ dx, dy };              // the displacement towards the target
        Point stepDxPoint{ stepDx, 0 };  // moves the monster horizontally one tile
        Point stepDyPoint{ 0, stepDy };  // moves the monster vertically one tile

        if (locationProvider.CanWalk(owner->GetLocation() + dp))
        {
            owner->SetLocation(owner->GetLocation() + dp);
        }
        else if (locationProvider.CanWalk(owner->GetLocation() + stepDxPoint))
        {
            owner->SetLocation(owner->GetLocation() + stepDxPoint);
        }
        else if (locationProvider.CanWalk(owner->GetLocation() + stepDyPoint))
        {
            owner->SetLocation(owner->GetLocation() + stepDyPoint);
        }
    }
    else if (owner->attacker)
    {
        owner->attacker->Attack(owner, locationProvider.GetPlayer());
    }
}

void ConfusedMonsterAi::Update(Actor* owner, ILocationProvider& locationProvider)
{
    // If the monster dies, we don't need special handling - the TemporaryAi will clean up
    if (owner->IsDead())
    {
        return;
    }

    // Move in a random direction
    TCODRandom* rng = TCODRandom::getInstance();
    Point dp{ rng->getInt(-1, 1), rng->getInt(-1, 1) };

    if (dp != Point::Zero)
    {
        Point destP{ owner->GetLocation() + dp };
        if (locationProvider.CanWalk(destP))
        {
            owner->SetLocation(destP);
        }
        else
        {
            // If we can't move there, try to attack whatever is blocking
            for (auto actor : locationProvider.GetActors())
            {
                if (actor->IsIn(destP) && !actor->IsDead())
                {
                    owner->Attack(actor);
                    break;
                }
            }
        }
    }
}