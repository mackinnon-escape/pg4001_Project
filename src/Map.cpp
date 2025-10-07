#include "Map.h"

#include "Engine.h"
#include "Actor.h"

static constexpr int ROOM_MAX_SIZE{ 12 };
static constexpr int ROOM_MIN_SIZE{ 6 };

Map::Map(int width, int height) : width(width), height(height)
{
    seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}

Map::~Map()
{
    delete[] tiles;
}

void Map::Init()
{
    TCODBsp bsp(0, 0, width, height);

    long area{ width * height };
    tiles = new Tile[area];

    BspCallback listener(*this);
    rng = new TCODRandom(seed, TCOD_RNG_CMWC);
    bsp.splitRecursive(rng, 8, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
    bsp.traverseInvertedLevelOrder(&listener, nullptr);
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

/*
* dig out the room: change Tile::canWalk to true
* Can also dig out a hallway, will do so if a coordinate is the same in both corners
*/
void Map::Dig(const Point& corner1, const Point& corner2) const
{
    Point UpperRight{ corner1 };
    Point LowerLeft{ corner2 };
    if (corner2.x < corner1.x)
    {
        UpperRight.x = corner2.x;
        LowerLeft.x = corner1.x;
    }
    if (corner2.y < corner1.y)
    {
        UpperRight.y = corner2.y;
        LowerLeft.y = corner1.y;
    }
    for (int tileX = UpperRight.x; tileX <= LowerLeft.x; tileX++)
    {
        for (int tileY = UpperRight.y; tileY <= LowerLeft.y; tileY++)
        {
            tiles[tileX + tileY * width].canWalk = true;
        }
    }
}

void Map::CreateRoom(const bool first, const Point& corner1, const Point& corner2) const
{
    Dig(corner1, corner2);
    Point middleOfRoom{ (corner1 + corner2) / 2 };

    // put the player in the first room
    if (first)
    {
        Engine::GetInstance()->player->SetLocation(middleOfRoom);
    }
}

bool BspCallback::visitNode(TCODBsp* node, void*)
{
    if (node->isLeaf())
    {
        // dig a room
        int w{ map.rng->getInt(ROOM_MIN_SIZE, node->w - 2) };
        int h{ map.rng->getInt(ROOM_MIN_SIZE, node->h - 2) };
        int x{ map.rng->getInt(node->x + 1, node->x + node->w - w - 1) };
        int y{ map.rng->getInt(node->y + 1, node->y + node->h - h - 1) };
        Point upperLeft{ x,y };
        Point lowerRight{ x + w - 1, y + h - 1 };
        map.CreateRoom(roomNumber == 0, upperLeft, lowerRight);

        Point roomMiddle{ x + w / 2, y + h / 2 };
        if (roomNumber != 0)
        {
            // dig a corridor from last room
            // hall goes from centre of previous room to middleHallPoint
            // and then on to the middle of the current room
            Point hallMiddle{ x + w / 2, previousLocation.y };
            map.Dig(previousLocation, hallMiddle);
            map.Dig(hallMiddle, roomMiddle);
        }

        previousLocation = roomMiddle;
        roomNumber++;
    }
    return true;
}