#include "Map.h"

#include "Engine.h"
#include "Point.h"

Map::Map(int width, int height) : width(width), height(height)
{
    long area{ width * height };
    tiles = new Tile[area];
}

Map::~Map()
{
    delete[] tiles;
}

void Map::Init()
{
    SetWall(Point{ 30,22 });
    SetWall(Point{ 50,22 });
}

bool Map::IsWall(const Point& position) const
{
    return !tiles[position.x + position.y * width].canWalk;
}

void Map::SetWall(const Point& p)
{
    tiles[p.x + p.y * width].canWalk = false;
}

void Map::Render() const
{
    static constexpr tcod::ColorRGB darkWall(0, 0, 100);
    static constexpr tcod::ColorRGB darkGround(50, 50, 150);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Point p{ x, y };
            Engine::GetInstance()->console.at(x, y).bg = IsWall(p) ? darkWall : darkGround;
        }
    }
}