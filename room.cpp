#include "room.hpp"

/*
    Room constructor - example call:
    Room("Kitchen", "kitchen", Room::MakeEmptyPaths(), std::unordered_map<Room::Message, std::string>{
        { Room::Message::OnEnter, "You have entered the kitchen." },
        { Room::Message::OnLook, "You are in the kitchen." }
    })
*/
Room::Room(
    std::string _name,
    std::string _repr,
    std::unordered_map<Direction, std::string> _paths,
    std::unordered_map<Message, std::string> _messages
) {
    name = _name;
    repr = _repr;
    paths = _paths;
    messages = _messages;
}

/* Room destructor, empty method */
Room::~Room() {}

/* internal name of the room */
std::string Room::getName() {
    return std::string(name);
}

/* in-game string representation of the room */
std::string Room::getRepr() {
    return std::string(repr);
}

/* gets a specific message type */
std::string Room::getMessage(Room::Message mtype) {
    return messages.at(mtype);
}

/*
    gets the name of the room in that direction from this room
    null value = empty string
*/
std::string Room::getPath(Direction d) {
    return std::string(paths.at(d));
}

/*
    sets the name of the room in that direction from this room
    null value = empty string
*/
void Room::setPath(Direction d, std::string other) {
    paths.at(d) = other;
}

/* get the list of names for every item in the room */
std::vector<std::string> Room::getItems() {
    return std::vector<std::string>(items);
}

/* adds an item's name to this.items */
void Room::addItem(std::string itemName) {
    items.push_back(itemName);
}

/* removes an item's name from this.items */
void Room::removeItem(std::string itemName) {
    items.erase(std::find(items.begin(), items.end(), itemName));
}
