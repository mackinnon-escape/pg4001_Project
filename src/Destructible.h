#pragma once

#include <string>
class Actor;

class Destructible
{
public:
    int maxHp;
    int hp;
    int defense;
    std::string corpseName;
    inline bool IsDead() const { return hp <= 0; }

    Destructible(int maxHp, int defense, const std::string& corpseName)
        : maxHp(maxHp), hp(maxHp), defense(defense), corpseName(corpseName) {
    }
    virtual ~Destructible() {}

    int TakeDamage(Actor* owner, int damage);
    virtual void Die(Actor* owner);
protected:
    enum DestructibleType
    {
        MONSTER, PLAYER
    };
};

class MonsterDestructible : public Destructible
{
public:
    MonsterDestructible(int maxHp, int defense, const std::string& corpseName)
        : Destructible(maxHp, defense, corpseName) {
    }
    void Die(Actor* owner);
};

class PlayerDestructible : public Destructible
{
public:
    PlayerDestructible(int maxHp, int defense, const std::string& corpseName)
        : Destructible(maxHp, defense, corpseName) {
    }
    void Die(Actor* owner);
};