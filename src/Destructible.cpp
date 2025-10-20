#include "Destructible.h"

#include "Colours.h"
#include "Actor.h"
#include "CustomEvents.h"


int Destructible::TakeDamage(Actor* owner, int damage)
{
    damage -= defense;
    if (damage > 0)
    {
        hp -= damage;
        NotifyHealthChanged();
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
    EventManager::GetInstance()->Publish(ActorDiedEvent(owner, corpseName));
}

// -----------------------------------------

void MonsterDestructible::Die(Actor* owner)
{
    EventManager::GetInstance()->Publish(MessageEvent(owner->name + " is dead.", LIGHT_GREY));
    Destructible::Die(owner);
}

// -----------------------------------------

PlayerDestructible::PlayerDestructible(int maxHp, int defense, const std::string& corpseName)
    : Destructible(maxHp, defense, corpseName) 
{
    NotifyHealthChanged();
}

void PlayerDestructible::Die(Actor* owner)
{
    EventManager::GetInstance()->Publish(MessageEvent("You died!", RED));
    Destructible::Die(owner);
}

void PlayerDestructible::NotifyHealthChanged() const
{
    EventManager::GetInstance()->Publish(HealthChangedEvent(hp, maxHp));
}