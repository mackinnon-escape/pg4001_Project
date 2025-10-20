#include "Ai.h"

#include "Actor.h"
#include "Engine.h"
#include "Map.h"
#include "Point.h"
#include "Gui.h"
#include "Colours.h"
#include "Input.h"
#include "CustomEvents.h"

void PlayerAi::Update(Actor* owner, ILocationProvider& locationProvider, Input& input)
{
    if (owner->IsDead())  return;
    owner->hasActedThisFrame = false;
    
    int dx{ 0 };
    int dy{ 0 };
    switch (input.GetKeyCode())
    {
    case SDLK_UP: dy = -1; break;
    case SDLK_DOWN: dy = 1; break;
    case SDLK_LEFT: dx = -1; break;
    case SDLK_RIGHT: dx = 1; break;
    default:
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
    for (auto actor : actorsInTarget)
    {
        if (actor->IsDead())
        {
            EventManager::GetInstance()->Publish(MessageEvent("There's a " + actor->name + " here", LIGHT_GREY));
        }
    }

    owner->SetLocation(target);
    return true;
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