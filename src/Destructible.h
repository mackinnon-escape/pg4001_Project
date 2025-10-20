#pragma once

#include <string>
class Actor;

class Destructible
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
protected:
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
};

class PlayerDestructible : public Destructible
{
public:
    PlayerDestructible(int maxHp, int defense, const std::string& corpseName);
    void Die(Actor* owner);
private:
    void NotifyHealthChanged() const override;
};
