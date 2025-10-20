#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include "libtcod.h"
#include "Point.h"
#include "ILocationProvider.h"

class Actor;
class Input;

struct Tile
{
    Tile() {}
    bool explored{ false }; // has the player already seen this tile ?
};

class Map : public ILocationProvider
{
public:
    Map(int width, int height, Input& input);
    virtual ~Map();

    void Init(bool withActors);
    void Render(tcod::Console& console) const;
    void Update();
    bool IsExplored(const Point& location) const;
    void ComputeFov() const;
    void DrawFirst(Actor* actor);
    // ILocatoinProvider
    bool IsWall(const Point& position) const override;
    bool CanWalk(const Point& position) const override;
    bool IsInFov(const Point& location) const override;
    Actor* GetPlayer() const override { return player; }
    Point GetPlayerLocation() const  override { return player->GetLocation(); }
    std::vector<Actor*> GetActorsAt(const Point& p) const override;

private:
    Tile* tiles{ nullptr };
    int width;
    int height;
    long seed;
    TCODRandom* rng{ nullptr };
    TCODMap* map{ nullptr };
    Input& inputHandler;
    Actor* player{ nullptr };
    std::vector<Actor*> actors{};
    friend class BspCallback;

    void Dig(const Point& corner1, const Point& corner2) const;
    void CreateRoom(const bool first, const Point& corner1, const Point& corner2, bool withActors);
    void AddMonster(const Point& location);
    void CreatePlayer();
};

class BspCallback : public ITCODBspCallback
{
public:
    BspCallback(Map& map) : map(map), roomNumber(0) {}
    bool visitNode(TCODBsp* node, void* userdata) override;

private:
    Map& map;                    // Reference to map to modify
    int roomNumber{ 0 };         // Room counter for first room detection
    Point previousLocation{};   // Center of the last room created
};
#endif // MAP_H