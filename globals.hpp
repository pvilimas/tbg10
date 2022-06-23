#ifndef __GLOBALS__
#define __GLOBALS__

#include <string>

#define REPR(x) (std::string((#x)))

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

std::string repr(Direction);
Direction reverse(Direction);

#endif /* __GLOBALS__ */