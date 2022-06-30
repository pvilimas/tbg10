#ifndef __ITEM__
#define __ITEM__

#include <string>
#include <vector>

#include "command.hpp"

/*
    An in-game item that the player can:
    - Pick up
    - Put down
    - Use
    - Inspect

    Either resides in a room or the player's inventory
*/
class Item {

    public:

    /*
        A set of item flags that determine how the player interacts with an item
        Default = {
            canCarry: false
        }
    */
    struct Flags {
        /*
            can the player carry it and drop it?
            note: tryDropItem does not check this flag, only if it's already in inventory
            the assumption being that inv starts empty
            
            TODO
            - add canUse, but maybe that should be governed by shit in attrs
            (like # of matches a matchbox has left, so you can light it)
        */
        bool canCarry;
    };

    /*
        A set of item attributes/properties that can change
        Default = {
            isFound: false
        }

        TODO see above
    */
    struct Attrs {
        bool isFound;
    };

    /*
        a type of item message, used to pass messages by type
    */
    enum Message {
        /* ex. look at lamp -> this is a small table lamp. */
        OnInspect,
    };

    private:

    /* internal name */
    std::string name;
    /* in-game string representation */
    std::string repr;
    /* contains all messages for this item */
    std::unordered_map<Message, std::string> messages;
    /* any special commands passed in, anything besides take and drop */
    std::vector<Command> specialCmds;
    /* the item attrs - see above */
    Item::Attrs itemAttrs;
    /* the item flags - see above */
    Item::Flags itemFlags;

    public:

    static inline constexpr Item::Attrs DefaultAttrs = Item::Attrs {
        .isFound = false
    };

    static inline constexpr Item::Flags DefaultFlags = Item::Flags {
        .canCarry = true
    };

    /*
        Item constructor
        _name: internal name
        _repr: in-game string representation
        _messages: all messages for this item (see Item::Message)
        _specialCmds: any special commands (like any uses)
        _attrs: the item attrs (properties)
        _flags: the item flags (mutable state)
    */
    Item(
        std::string _name,
        std::string _repr,
        std::unordered_map<Message, std::string> _messages,
        std::vector<Command> _specialCmds = {},
        Item::Attrs _attrs = Item::DefaultAttrs,
        Item::Flags _flags = Item::DefaultFlags
    );

    /*  the internal name of the item  */
    std::string& GetName();

    /*  the in-game string representation of the item */ 
    std::string& GetRepr();

    /*  return a specific message  */
    std::string& GetMessage(Message mtype);

    /*  get list of all special commands  */
    std::vector<Command>& GetSpecialCommands();

    /*  all item attributes (properties)  */
    Item::Attrs& GetAttrs();

    /*  all item flags (mutable state)  */
    Item::Flags& GetFlags();

};

#endif /* __ITEM__ */