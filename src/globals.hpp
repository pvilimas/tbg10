#ifndef __GLOBALS__
#define __GLOBALS__

#include <string>

/*
    NOTE - tried reworking direction into a class, isn't possible
    https://discord.com/channels/331718482485837825/845027465419161621/992081218004865104
*/

/*
    Represents a direction the player can move in
*/
enum class Direction {
    North,
    South,
    East,
    West,
    Invalid
};

constexpr inline std::string_view direction_reprs[2][5] = {
    { "North", "South", "East", "West", "INVALID" },
    { "north", "south", "east", "west", "invalid" }
};

constexpr inline Direction direction_revs[5] = {
    Direction::South,
    Direction::North,
    Direction::West,
    Direction::East,
    Direction::Invalid
};

/*  in-game string representation  */
std::string DirectionRepr(Direction const &d, bool uppercase);
/*  the opposite way  */
Direction DirectionReverse(Direction const &d);

#endif /* __GLOBALS__ */