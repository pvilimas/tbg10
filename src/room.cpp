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

std::string& Room::GetName() {
    return name;
}

std::string& Room::GetRepr() {
    return repr;
}

std::string& Room::GetMessage(Room::Message mtype) {
    return messages.at(mtype);
}

std::string& Room::GetPath(Direction d) {
    return paths.at(d);
}

void Room::SetPath(Direction d, std::string other) {
    paths.at(d) = other;
}

std::vector<std::string>& Room::GetItems() {
    return items;
}

void Room::AddItem(std::string itemName) {
    items.push_back(itemName);
}

void Room::RemoveItem(std::string itemName) {
    items.erase(std::find(items.begin(), items.end(), itemName));
}
