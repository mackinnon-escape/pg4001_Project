#ifndef COLOURS_H
#define COLOURS_H

#include "libtcod.h"

// Color Definitions using Macros
constexpr tcod::ColorRGB LIGHT_RED{ 255, 63, 63 };
constexpr tcod::ColorRGB RED{ 191, 0, 0 };
constexpr tcod::ColorRGB DARK_RED{ 255, 0, 0 };
constexpr tcod::ColorRGB DARKER_RED{ 127, 0, 0 };
constexpr tcod::ColorRGB LIGHT_YELLOW{ 255, 255, 63 };
constexpr tcod::ColorRGB YELLOW{ 255, 255, 0 };
constexpr tcod::ColorRGB LIGHT_VIOLET{ 159, 63, 255 };
constexpr tcod::ColorRGB VIOLET{ 127, 0, 255 };
constexpr tcod::ColorRGB DARKER_VIOLET{ 63, 0, 127 };
constexpr tcod::ColorRGB CYAN{ 0, 255, 255 };
constexpr tcod::ColorRGB LIGHTER_ORANGE{ 255, 191, 127 };
constexpr tcod::ColorRGB DESATURATED_GREEN{ 63, 127, 63 };
constexpr tcod::ColorRGB DARKER_GREEN{ 0, 127, 0 };

constexpr tcod::ColorRGB WHITE{ 255, 255, 255 };
constexpr tcod::ColorRGB LIGHT_GREY{ 159, 159, 159 };
constexpr tcod::ColorRGB BLACK{ 0, 0, 0 };

#endif // COLOURS_H