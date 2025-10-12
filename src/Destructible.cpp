#include "Destructible.h"


#include "Engine.h"
#include "Colours.h"
#include "Actor.h"

int Destructible::TakeDamage(Actor* owner, int damage)
{
    damage -= defense;
    if (damage > 0)
    {
        hp -= damage;
        if (hp <= 0)
        {
            Die(owner);
        }
    }
    else
    {
        damage = 0;
    }
    return damage;
}

void Destructible::Die(Actor* owner)
{
    owner->ChangeToCorpse(corpseName);
    // make sure corpses are drawn before living actors
    Engine::GetInstance()->DrawFirst(owner);
}

// -----------------------------------------

void MonsterDestructible::Die(Actor* owner)
{
    // transform it into a nasty corpse! it doesn't block, can't be attacked and doesn't move
    Destructible::Die(owner);
}

// -----------------------------------------

void PlayerDestructible::Die(Actor* owner)
{
    Destructible::Die(owner);
    Engine::GetInstance()->gameStatus = Engine::DEFEAT;
}