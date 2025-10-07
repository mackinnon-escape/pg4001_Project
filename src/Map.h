#ifndef MAP_H
#define MAP_H

#include <string>
#include "libtcod.h"
#include "Point.h"

struct Tile
{
    Tile() {}
    bool canWalk{ false };
};

class Map
{
public:
    Map(int width, int height);
    virtual ~Map();

    void Init();
    bool IsWall(const Point& position) const;
    void Render() const;
private:
    Tile* tiles{ nullptr };
    int width;
    int height;
    long seed;                    // NEW: Random seed for reproducible generation
    TCODRandom* rng{ nullptr };     // NEW: Random number generator
    friend class BspCallback;    // NEW: Allow BspCallback access to private members

    void SetWall(const Point& p);
    void Dig(const Point& corner1, const Point& corner2) const;        // NEW
    void CreateRoom(const bool first, const Point& corner1, const Point& corner2) const;  // NEW
};

class BspCallback : public ITCODBspCallback
{
private:
    Map& map;                    // Reference to map to modify
    int roomNumber;                 // Room counter for first room detection
    Point previousLocation{};   // Center of the last room created

public:
    BspCallback(Map& map) : map(map), roomNumber(0) {}
    bool visitNode(TCODBsp* node, void* userdata) override;
};
#endif // MAP_H