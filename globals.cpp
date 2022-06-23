#include "globals.hpp"

/*
    Get the in-game string representation of a Direction
*/
std::string repr(Direction d) {
    switch(d) {
        case North: return "north";
        case South: return "south";
        case East: return "east";
        case West: return "west";
        default: return "invalid_dir";
    }
}

/*
    Get the reverse of a direction, the opposite way
*/
Direction reverse(Direction d) {
    switch(d) {
        case North: return South;
        case South: return North;
        case East: return West;
        case West: return East;
        default: return Invalid;
    }
}