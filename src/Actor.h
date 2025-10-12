#ifndef ACTOR_H
#define ACTOR_H

#include "libtcod.h"

#include "Point.h"
#include "Colours.h"
#include "Destructible.h"
#include "Attacker.h"
#include "Ai.h"

class Actor
{
public:
    Actor() {}
    Actor(const Point initialLocation, const int ch, std::string name, const TCODColor& col) : position(initialLocation), ch(ch), name(name), colour(col) {}
    virtual ~Actor() = default;

    void Update();
    void Render() const;
    bool Move(const Point& target);

    Point GetLocation() { return position; }
    void SetLocation(const Point& cp) { position = cp; }
    bool IsIn(const Point& target) const { return position == target; }
    bool IsInSameLocationAs(const Actor* other) const { return IsIn(other->position); }
    bool IsInSameRowAs(const Actor* other) const { return position.y == other->position.y; }
    bool IsInSameColAs(const Actor* other) const { return position.x == other->position.x; }
    void ChangeToCorpse(std::string corpseName);
    bool IsDead() const { return destructible && destructible->IsDead(); }
    int TakeDamage(int damage);
    bool IsAlive() const { return destructible && !destructible->IsDead(); }
    void Attack(const Actor* owner, Actor* target) const { if (attacker) attacker->Attack(owner, target); }
    int GetDefense() const { return destructible ? destructible->defense : 0; }

    Destructible* destructible{ nullptr }; // something that can be damaged
    Attacker* attacker{ nullptr };
    std::unique_ptr<Ai> ai{ nullptr };     // something self-updating

    std::string name{};

private:
    Point position{ Point::Zero };
    int ch{ 0 }; // ascii code
    TCODColor colour{ WHITE };
    bool blocks{ true };
};
#endif  // ACTOR_H