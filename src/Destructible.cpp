#include "Destructible.h"

#include "Colours.h"
#include "Actor.h"
#include "CustomEvents.h"
#include "Serialise.h"

Destructible* Destructible::Create(Loader& loader)
{
    DestructibleType type = static_cast<DestructibleType>(loader.GetInt());
    Destructible* destructible{};
    switch (type)
    {
    case MONSTER: destructible = new MonsterDestructible(0, 0, ""); break;
    case PLAYER: destructible = new PlayerDestructible(0, 0, ""); break;
    }
    destructible->Load(loader);
    return destructible;
}

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

int Destructible::Heal(int amount)
{
    hp += amount;
    if (hp > maxHp)
    {
        amount -= hp - maxHp;
        hp = maxHp;
    }

    if (amount != 0)
    {
        NotifyHealthChanged();
    }

    return amount;
}

void Destructible::Save(Saver& saver) const
{
    saver.PutInt(maxHp);
    saver.PutInt(hp);
    saver.PutInt(defense);
    saver.PutString(corpseName);
}

void Destructible::Load(Loader& loader)
{
    maxHp = loader.GetInt();
    hp = loader.GetInt();
    defense = loader.GetInt();
    corpseName = loader.GetString();
}
// -----------------------------------------

void MonsterDestructible::Die(Actor* owner)
{
    EventManager::GetInstance()->Publish(MessageEvent(owner->name + " is dead.", LIGHT_GREY));
    Destructible::Die(owner);
}

void MonsterDestructible::Save(Saver& saver) const
{
    saver.PutInt(MONSTER);
    Destructible::Save(saver);
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

void PlayerDestructible::Save(Saver& saver) const
{
    saver.PutInt(PLAYER);
    Destructible::Save(saver);
}

void PlayerDestructible::Load(Loader& loader)
{
    // Type extracted in Create()
    Destructible::Load(loader);
    NotifyHealthChanged();
}