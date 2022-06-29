#ifndef __GLOBALS__
#define __GLOBALS__

#include <string>

/*
    TODO
    rework globals into a class, with static instances maybe
    and rename to direction
    2 methods: repr and reverse
*/

/*
    Represents a direction the player can move in
*/
enum Direction {
    North,
    South,
    East,
    West,
    Invalid
};

/*
    Get the in-game string representation of a Direction
*/
std::string repr(Direction);

/*
    Get the reverse of a direction, the opposite way
*/
Direction reverse(Direction);

#endif /* __GLOBALS__ */