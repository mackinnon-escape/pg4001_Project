#ifndef ACTOR_H
#define ACTOR_H

#include "libtcod.h"

#include "Point.h"
#include "Colours.h"

class Actor
{
public:
    Actor() {}
    Actor(const Point initialLocation, const int ch, const TCODColor& col) : position(initialLocation), ch(ch), colour(col) {}
    virtual ~Actor() = default;

    void Render() const;

    Point GetLocation() { return position; }
    void SetLocation(const Point& cp) { position = cp; }
    bool IsIn(const Point& target) const { return position == target; }
    bool IsInSameLocationAs(const Actor* other) const { return IsIn(other->position); }
    bool IsInSameRowAs(const Actor* other) const { return position.y == other->position.y; }
    bool IsInSameColAs(const Actor* other) const { return position.x == other->position.x; }

private:
    Point position{ Point::Zero };
    int ch{ 0 }; // ascii code
    TCODColor colour{ WHITE };
};
#endif  // ACTOR_H