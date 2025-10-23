#pragma once

#include <vector>
#include "IPersistable.h"

class Actor;

class Container : IPersistable
{
public:
    int maxSize; // maximum number of actors. 0=unlimited
    std::vector<Actor*> inventory{};

    Container(int size) : maxSize(size) {}
    virtual ~Container();
    bool Add(Actor* actor);
    void Remove(Actor* actor);

    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;
private:
};