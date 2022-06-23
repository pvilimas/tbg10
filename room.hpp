#ifndef __ROOM__
#define __ROOM__

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "globals.hpp"

class Room {

    public:

    enum Message {
        OnEnter,
        OnLook,
    };

    private:
    
    /* the internal name of the room */
    std::string name;
    /* the in-game string representation of the room */
    std::string repr;
    /*
        map of which rooms are in each direction (currently just north south east west)
        key = enum Direction, value = room name (or "" for no path going that way)
    */
    std::unordered_map<Direction, std::string> paths;
    /*
        contains all messages for this room, uses Room::Message as the key, see above
    */
    std::unordered_map<Message, std::string> messages;
    /*
        a list of all item names within this room
        should try to avoid duplicates, that can fuck shit up
    */
    std::vector<std::string> items;

    public:

    /*
        avoids mutability issues/accidental shared instance, makes a new one every time
        called as a constructor argument, not a default arg
    */
    static inline auto MakeEmptyPaths = []{
        return std::unordered_map<Direction, std::string>({
            { Direction::North, "" },
            { Direction::South, "" },
            { Direction::East, "" },
            { Direction::West, "" },
        });
    };

    Room(
        std::string _name,
        std::string _repr,
        std::unordered_map<Direction, std::string> _paths,
        std::unordered_map<Message, std::string> _messages
    );
    ~Room();

    std::string getName();
    std::string getRepr();

    std::string getMessage(Message mtype);

    std::string getPath(Direction d);
    void setPath(Direction d, std::string other);

    std::vector<std::string> getItems();
    void addItem(std::string itemName);
    void removeItem(std::string itemName);

};

#endif /* __ROOM__ */