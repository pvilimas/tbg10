#include "item.hpp"

Item::Item(
    std::string _name,
    std::string _repr,
    std::unordered_map<Message, std::string> _messages,
    std::vector<Command> _specialCmds,
    Item::Attrs _attrs,
    Item::Flags _flags
) {
    name = _name;
    repr = _repr;
    messages = _messages;
    specialCmds = _specialCmds;
    itemAttrs = _attrs;
    itemFlags = _flags;
}

std::string& Item::GetName() {
    return name;
}

std::string& Item::GetRepr() {
    return repr;
}

std::string& Item::GetMessage(Item::Message mtype) {
    return messages.at(mtype);
}

std::vector<Command>& Item::GetSpecialCommands() {
    return specialCmds;
}

Item::Attrs& Item::GetAttrs() {
    return itemAttrs;
}

Item::Flags& Item::GetFlags() {
    return itemFlags;
}