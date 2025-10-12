#include "Ai.h"

#include "Actor.h"
#include "Engine.h"
#include "Map.h"
#include "Point.h"
#include "Gui.h"
#include "Colours.h"

void PlayerAi::Update(Actor* owner)
{
    if (owner->IsDead())  return;

    int dx{ 0 };
    int dy{ 0 };
    switch (Engine::GetInstance()->GetKeyCode())
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
        Engine::GetInstance()->gameStatus = Engine::NEW_TURN;
        if (MoveOrAttack(owner, owner->GetLocation() + dp))
        {
            Engine::GetInstance()->map->ComputeFov();
        }
    }
}

bool PlayerAi::MoveOrAttack(Actor* owner, const Point& target)
{
    if (Engine::GetInstance()->map->IsWall(target)) return false;
    // look for living actors to attack
    for (auto actor : Engine::GetInstance()->actors)
    {
        if (actor->IsAlive() && actor->IsIn(target))
        {
            owner->Attack(owner, actor);
            return false;
        }
    }

    // look for corpses or items
    for (auto actor : Engine::GetInstance()->actors)  // new section
    {
        bool corpse = actor->IsDead();
        if (corpse && actor->IsIn(target))
        {
            Engine::GetInstance()->gui->SendMessage(LIGHT_GREY, "There's a %s here", actor->name.c_str());
        }
    }

    owner->SetLocation(target);
    return true;
}

// how many turns the monster chases the player
// after losing his sight
static constexpr int TRACKING_TURNS{ 3 };

void MonsterAi::Update(Actor* owner)
{
    if (owner->IsDead()) return;

    if (Engine::GetInstance()->map->IsInFov(owner->GetLocation()))
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
        MoveOrAttack(owner, Engine::GetInstance()->player->GetLocation());
    }
}

void MonsterAi::MoveOrAttack(Actor* owner, const Point& target)
{
    int dx{ target.x - owner->GetLocation().x };
    int dy{ target.y - owner->GetLocation().y };
    int stepDx{ dx > 0 ? 1 : -1 };
    int stepDy{ dy > 0 ? 1 : -1 };
    float distance{ sqrtf(static_cast<float>(dx * dx + dy * dy)) };
    if (distance >= 2)
    {
        dx = static_cast<int>(round(static_cast<float>(dx) / distance));
        dy = static_cast<int>(round(static_cast<float>(dy) / distance));
        Point dp{ dx, dy };              // the displacement towards the target
        Point stepDxPoint{ stepDx, 0 };  // moves the monster horizontally one tile
        Point stepDyPoint{ 0, stepDy };  // moves the monster vertically one tile

        if (Engine::GetInstance()->map->CanWalk(owner->GetLocation() + dp))
        {
            owner->SetLocation(owner->GetLocation() + dp);
        }
        else if (Engine::GetInstance()->map->CanWalk(owner->GetLocation() + stepDxPoint))
        {
            owner->SetLocation(owner->GetLocation() + stepDxPoint);
        }
        else if (Engine::GetInstance()->map->CanWalk(owner->GetLocation() + stepDyPoint))
        {
            owner->SetLocation(owner->GetLocation() + stepDyPoint);
        }
    }
    else if (owner->attacker)
    {
        owner->attacker->Attack(owner, Engine::GetInstance()->player);
    }
}