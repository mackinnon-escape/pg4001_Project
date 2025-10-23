#pragma once

#include <memory>
#include "IPersistable.h"

struct Point;
class Input;
class ILocationProvider;
class Actor;

enum class AiType
{
    PLAYER,
    MONSTER,
    CONFUSED_MONSTER,
    TEMPORARY
};

class Ai : IPersistable
{
public:
    virtual ~Ai() {};

    virtual void Update(Actor*, ILocationProvider&) {};
    virtual void Update(Actor* owner, ILocationProvider& locationProvider, Input&) { Update(owner, locationProvider); }

    virtual AiType GetType() const = 0;
    void Save(Saver& saver) const override;
    void Load(Loader&) override {}
    static std::unique_ptr<Ai> Create(Loader& loader);
protected:
};

class PlayerAi : public Ai
{
public:
    PlayerAi() {};
    void Update(Actor* owner, ILocationProvider& locationProvider, Input& input) override;
    AiType GetType() const override { return AiType::PLAYER; }
protected:
    static bool MoveOrAttack(Actor* owner, const Point& target, ILocationProvider& locationProvider);
    bool HandleActionKey(Actor* owner, unsigned int ascii, ILocationProvider& locationProvider, Input& input);
};

class MonsterAi : public Ai
{
public:
    void Update(Actor* owner, ILocationProvider& locationProvider) override;
    AiType GetType() const override { return AiType::MONSTER; }
    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;

protected:
    int moveCount{ 0 };
    static void MoveOrAttack(Actor* owner, const Point& target, ILocationProvider& locationProvider);
};

class ConfusedMonsterAi : public Ai
{
public:
    ConfusedMonsterAi() {}
    virtual ~ConfusedMonsterAi() = default;
    
    void Update(Actor* owner, ILocationProvider& locationProvider) override;
    AiType GetType() const override { return AiType::CONFUSED_MONSTER; }
};
