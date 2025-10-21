#pragma once

#include <vector>
class Actor;

class Container
{
public:
    int maxSize; // maximum number of actors. 0=unlimited
    std::vector<Actor*> inventory{};

    Container(int size) : maxSize(size) {}
    virtual ~Container();
    bool Add(Actor* actor);
    void Remove(Actor* actor);
private:
};