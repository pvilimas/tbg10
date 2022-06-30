#include "globals.hpp"

std::string ReprDirection(Direction& d) {
    switch(d) {
        case Direction::North: return "north";
        case Direction::South: return "south";
        case Direction::East: return "east";
        case Direction::West: return "west";
        default: return "invalid_dir";
    }
}

Direction ReverseDirection(Direction& d) {
    switch(d) {
        case Direction::North: return Direction::South;
        case Direction::South: return Direction::North;
        case Direction::East: return Direction::West;
        case Direction::West: return Direction::East;
        default: return Direction::Invalid;
    }
}