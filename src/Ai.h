#pragma once

class Actor;
struct Point;

class Ai
{
public:
    virtual ~Ai() {};

    virtual void Update(Actor* owner) = 0;
protected:
};

class PlayerAi : public Ai
{
public:
    PlayerAi() {};
    void Update(Actor* owner) override;

protected:
    static bool MoveOrAttack(Actor* owner, const Point& target);
};

class MonsterAi : public Ai
{
public:
    void Update(Actor* owner) override;

protected:
    int moveCount{ 0 };
    static void MoveOrAttack(Actor* owner, const Point& target);
};