#ifndef __ITEM__
#define __ITEM__

#include <string>
#include <vector>

#include "command.hpp"

class Item {

    public:

    struct Flags {
        bool canCarry;
    };

    struct Attrs {
        bool isFound;
    };

    enum Message {
        OnInspect,
    };

    private:

    std::string name;
    std::string repr;
    std::unordered_map<Message, std::string> messages;
    std::vector<Command> specialCmds;
    Attrs itemAttrs;
    Flags itemFlags;

    public:

    static Flags flags(bool _canCarry);
    static Attrs attrs(bool isFound);

    Item(
        std::string _name,
        std::string _repr,
        std::unordered_map<Message, std::string> _messages,
        std::vector<Command> _specialCmds = {},
        Attrs _attrs = { false },
        Flags _flags = { true }
    );

    std::string getName();
    std::string getRepr();
    std::string getMessage(Message mtype);
    std::vector<Command> getSpecialCommands();
    Attrs &getAttrs();
    Flags &getFlags();
};

#endif /* __ITEM__ */