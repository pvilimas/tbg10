#include "item.hpp"

Item::Flags Item::flags(bool _canCarry) {
    return Flags { _canCarry };
}

Item::Attrs Item::attrs(bool isFound) {
    return Attrs { isFound };
}

Item::Item(
    std::string _name,
    std::string _repr,
    std::unordered_map<Message, std::string> _messages,
    std::vector<Command> _specialCmds,
    Attrs _attrs,
    Flags _flags
) {
    name = _name;
    repr = _repr;
    messages = _messages;
    specialCmds = _specialCmds;
    itemAttrs = _attrs;
    itemFlags = _flags;
}

std::string Item::getName() {
    return name;
}

std::string Item::getRepr() {
    return repr;
}

std::string Item::getMessage(Item::Message mtype) {
    return messages.at(mtype);
}

std::vector<Command> Item::getSpecialCommands() {
    return std::vector<Command>(specialCmds);
}

Item::Attrs &Item::getAttrs() {
    return itemAttrs;
}

Item::Flags &Item::getFlags() {
    return itemFlags;
}