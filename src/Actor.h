#ifndef ACTOR_H
#define ACTOR_H

#include "libtcod.h"

#include "Point.h"
#include "Colours.h"
#include "Destructible.h"
#include "Attacker.h"
#include "Ai.h"
#include "Pickable.h"
#include "Container.h"
#include "IPersistable.h"

class Input;
class ILocationProvider;

class Actor : IPersistable
{
public:
    Actor() {}
    Actor(const Point initialLocation, const int ch, std::string name, const TCODColor& col) : position(initialLocation), ch(ch), name(name), colour(col) {}
    virtual ~Actor();

    void Update(Input& input, ILocationProvider& locationProvider);
    void Render(tcod::Console& console) const;

    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
    
    Point GetLocation() { return position; }
    void SetLocation(const Point& cp) { position = cp; }
    bool IsIn(const Point& target) const { return position == target; }
    bool IsInSameLocationAs(const Actor* other) const { return IsIn(other->position); }
    bool IsInSameRowAs(const Actor* other) const { return position.y == other->position.y; }
    bool IsInSameColAs(const Actor* other) const { return position.x == other->position.x; }
    float GetDistanceFrom(const Point& target) const { return position.DistanceTo(target); }

    void ChangeToCorpse(std::string corpseName);
    bool IsDead() const { return destructible && destructible->IsDead(); }
    int TakeDamage(int damage);
    bool IsAlive() const { return destructible && !destructible->IsDead(); }
    void Attack(Actor* target) const { if (attacker) attacker->Attack(this, target); }
    int GetDefense() const { return destructible ? destructible->defense : 0; }
    int Heal(int amount) { return destructible != nullptr ? destructible->Heal(amount) : 0; }
    bool AddToContainer(Actor* item) { return container ? container->Add(item) : false; }
    bool RemoveFromContainer(Actor* item);

    Destructible* destructible{ nullptr }; // something that can be damaged
    Attacker* attacker{ nullptr };
    std::unique_ptr<Ai> ai{ nullptr };     // something self-updating
    Pickable* pickable{ nullptr };         // something that can be picked and used
    Container* container{ nullptr };       // something that can contain actors

    std::string name{};
    bool blocks{ true };
    bool hasActedThisFrame{ false };
private:
    Point position{ Point::Zero };
    int ch{ 0 }; // ascii code
    TCODColor colour{ WHITE };
};
#endif  // ACTOR_H