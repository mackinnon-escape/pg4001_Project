#pragma once

class Actor;
struct Point;
class Input;
class ILocationProvider;

class Ai
{
public:
    virtual ~Ai() {};

    virtual void Update(Actor*, ILocationProvider&) {};
    virtual void Update(Actor* owner, ILocationProvider& locationProvider, Input&) { Update(owner, locationProvider); }
protected:
};

class PlayerAi : public Ai
{
public:
    PlayerAi() {};
    void Update(Actor* owner, ILocationProvider& locationProvider, Input& input) override;

protected:
    static bool MoveOrAttack(Actor* owner, const Point& target, ILocationProvider& locationProvider);
    bool HandleActionKey(Actor* owner, unsigned int ascii, ILocationProvider& locationProvider, Input& input);
};

class MonsterAi : public Ai
{
public:
    void Update(Actor* owner, ILocationProvider& locationProvider) override;

protected:
    int moveCount{ 0 };
    static void MoveOrAttack(Actor* owner, const Point& target, ILocationProvider& locationProvider);
};