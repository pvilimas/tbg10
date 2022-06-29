#include "globals.hpp"

std::string repr(Direction d) {
    switch(d) {
        case North: return "north";
        case South: return "south";
        case East: return "east";
        case West: return "west";
        default: return "invalid_dir";
    }
}

Direction reverse(Direction d) {
    switch(d) {
        case North: return South;
        case South: return North;
        case East: return West;
        case West: return East;
        default: return Invalid;
    }
}