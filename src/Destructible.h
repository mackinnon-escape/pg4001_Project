#pragma once

#include <string>
#include "IPersistable.h"

class Actor;

enum class LevelBoost
{
    CONSTITUTION,
    STRENGTH,
    AGILITY
};

class Destructible : IPersistable
{
public:
    int defense;
    std::string corpseName;
    inline bool IsDead() const { return hp <= 0; }

    Destructible(int maxHp, int defense, int xp, const std::string& corpseName)
        : corpseName(corpseName), defense(defense), maxHp(maxHp), hp(maxHp), xp(xp) {}
    virtual ~Destructible() {}

    int TakeDamage(Actor* owner, int damage);
    virtual void Die(Actor* owner);
    int Heal(int amount);
    // IPersistable
    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
    static Destructible* Create(Loader& loader);
    void ApplyLevelBoost(Actor* owner, LevelBoost boost);
protected:
    enum DestructibleType
    {
        MONSTER, PLAYER
    };
    virtual void NotifyHealthChanged() const {}
    void BoostHp();
    void BoostDefense() { defense++; }

    int maxHp;
    int hp;
    int xp;
};

class MonsterDestructible : public Destructible
{
public:
    MonsterDestructible(int maxHp, int defense, int xp, const std::string& corpseName)
        : Destructible(maxHp, defense, xp, corpseName) {
    }
    void Die(Actor* owner);
    void Save(Saver& saver) const override;
};

class PlayerDestructible : public Destructible
{
public:
    PlayerDestructible(int maxHp, int defense, const std::string& corpseName);
    void Die(Actor* owner);
    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
private:
    void NotifyHealthChanged() const override;
    void SubscribeToEvents();
    int GetNextLevelXp() const;
    int level{ 1 };
};
