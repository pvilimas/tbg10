#ifndef __GLOBALS__
#define __GLOBALS__

#include <string>

#define REPR(x) (std::string((#x)))

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