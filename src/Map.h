#ifndef MAP_H
#define MAP_H

#include <string>
#include "libtcod.h"
#include "Point.h"

struct Tile
{
    Tile() {}
    bool explored{ false }; // has the player already seen this tile ?
};

class Map
{
public:
    Map(int width, int height);
    virtual ~Map();

    void Init();
    bool IsWall(const Point& position) const;
    void Render() const;
    bool IsInFov(const Point& location) const;
    bool IsExplored(const Point& location) const;
    void ComputeFov() const;
private:
    Tile* tiles{ nullptr };
    int width;
    int height;
    long seed;
    TCODRandom* rng{ nullptr };
    TCODMap* map{ nullptr };
    friend class BspCallback;

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