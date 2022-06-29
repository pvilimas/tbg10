#include "room.hpp"

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

Room::~Room() {}

std::string Room::getName() {
    return std::string(name);
}

std::string Room::getRepr() {
    return std::string(repr);
}

std::string Room::getMessage(Room::Message mtype) {
    return messages.at(mtype);
}

std::string Room::getPath(Direction d) {
    return std::string(paths.at(d));
}

void Room::setPath(Direction d, std::string other) {
    paths.at(d) = other;
}

std::vector<std::string> Room::getItems() {
    return std::vector<std::string>(items);
}

void Room::addItem(std::string itemName) {
    items.push_back(itemName);
}

void Room::removeItem(std::string itemName) {
    items.erase(std::find(items.begin(), items.end(), itemName));
}
