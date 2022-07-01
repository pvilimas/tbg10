#include "globals.hpp"

std::string DirectionRepr(Direction const &d, bool uppercase) {
    return std::string(direction_reprs[uppercase][static_cast<int>(d)]);
}

Direction DirectionReverse(Direction const &d) {
    return direction_revs[static_cast<int>(d)];
}