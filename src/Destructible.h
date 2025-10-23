#pragma once

#include <string>
#include "IPersistable.h"

class Actor;

class Destructible : IPersistable
{
public:
    int defense;
    std::string corpseName;
    inline bool IsDead() const { return hp <= 0; }

    Destructible(int maxHp, int defense, const std::string& corpseName)
        : maxHp(maxHp), hp(maxHp), defense(defense), corpseName(corpseName) {
    }
    virtual ~Destructible() {}

    int TakeDamage(Actor* owner, int damage);
    virtual void Die(Actor* owner);
    int Heal(int amount);
    // IPersistable
    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
    static Destructible* Create(Loader& loader);
protected:
    enum DestructibleType
    {
        MONSTER, PLAYER
    };
    virtual void NotifyHealthChanged() const {}

    int maxHp;
    int hp;
};

class MonsterDestructible : public Destructible
{
public:
    MonsterDestructible(int maxHp, int defense, const std::string& corpseName)
        : Destructible(maxHp, defense, corpseName) {
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
};
