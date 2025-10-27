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
    case MONSTER: destructible = new MonsterDestructible(0, 0, 0, ""); break;
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

void Destructible::ApplyLevelBoost(Actor* owner, LevelBoost boost)
{
    switch (boost)
    {
    case LevelBoost::CONSTITUTION:
        // Constitution: +20 HP
        BoostHp();
        EventManager::GetInstance()->Publish(HealthChangedEvent(hp, maxHp));
        EventManager::GetInstance()->Publish(MessageEvent("Constitution increased! (+20 HP)", DESATURATED_GREEN));
        break;
    case LevelBoost::STRENGTH:
        // Strength: +1 attack
        if (owner->attacker)
        {
            owner->BoostPower();
            EventManager::GetInstance()->Publish(MessageEvent("Strength increased! (+1 attack)", DESATURATED_GREEN));
        }
        break;
    case LevelBoost::AGILITY:
        // Agility: +1 defense
        BoostDefense();
        EventManager::GetInstance()->Publish(MessageEvent("Agility increased! (+1 defense)", DESATURATED_GREEN));
    }
}

void Destructible::BoostHp()
{
    maxHp += 20;
    hp += 20;
}

void Destructible::Save(Saver& saver) const
{
    saver.PutInt(maxHp);
    saver.PutInt(hp);
    saver.PutInt(xp);
    saver.PutInt(defense);
    saver.PutString(corpseName);
}

void Destructible::Load(Loader& loader)
{
    maxHp = loader.GetInt();
    hp = loader.GetInt();
    xp = loader.GetInt();
    defense = loader.GetInt();
    corpseName = loader.GetString();
}
// -----------------------------------------

void MonsterDestructible::Die(Actor* owner)
{
    EventManager::GetInstance()->Publish(MessageEvent(owner->name + " is dead. You gain " + std::to_string(xp) + " xp", LIGHT_GREY));
    Destructible::Die(owner);
    EventManager::GetInstance()->Publish(XPGainedEvent(xp));
}

void MonsterDestructible::Save(Saver& saver) const
{
    saver.PutInt(MONSTER);
    Destructible::Save(saver);
}

// -----------------------------------------
constexpr int LEVEL_UP_BASE{ 200 };
constexpr int LEVEL_UP_FACTOR{ 150 };

PlayerDestructible::PlayerDestructible(int maxHp, int defense, const std::string& corpseName)
    : Destructible(maxHp, defense, 0, corpseName) 
{
    SubscribeToEvents();

    NotifyHealthChanged();
    EventManager::GetInstance()->Publish(UpdateLevelAndXPEvent(level, xp, GetNextLevelXp()));
}

void PlayerDestructible::Die(Actor* owner)
{
    Destructible::Die(owner);
    EventManager::GetInstance()->Publish(MessageEvent("You died!", RED));
    EventManager::GetInstance()->Publish(GameOverEvent("You died!"));
}

void PlayerDestructible::NotifyHealthChanged() const
{
    EventManager::GetInstance()->Publish(HealthChangedEvent(hp, maxHp));
}

void PlayerDestructible::SubscribeToEvents()
{
    EventManager::GetInstance()->Subscribe(EventType::XPGained,
        [&, this](const Event& e)
        {
            const auto& xpEvent = static_cast<const XPGainedEvent&>(e);
            xp += xpEvent.xpAmount;
            // Check for level up
            int levelUpXp = GetNextLevelXp();
            if (xp >= levelUpXp)
            {
                level++;
                xp -= levelUpXp;
                EventManager::GetInstance()->Publish(MessageEvent("Your battle skills grow stronger! You reached level " + std::to_string(level), YELLOW));
                EventManager::GetInstance()->Publish(LevelChangingEvent(level));
            }
            // Always publish level changed event to update GUI
            EventManager::GetInstance()->Publish(UpdateLevelAndXPEvent(level, xp, GetNextLevelXp()));
        });
}

int PlayerDestructible::GetNextLevelXp() const
{
    return LEVEL_UP_BASE + level * LEVEL_UP_FACTOR;
}

void PlayerDestructible::Save(Saver& saver) const
{
    saver.PutInt(PLAYER);
    Destructible::Save(saver);
    saver.PutInt(level);
}

void PlayerDestructible::Load(Loader& loader)
{
    // Type extracted in Create()
    Destructible::Load(loader);
    NotifyHealthChanged();
    level = loader.GetInt();

    EventManager::GetInstance()->Publish(UpdateLevelAndXPEvent(level, xp, GetNextLevelXp()));
}