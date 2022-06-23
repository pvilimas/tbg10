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
    
    std::string name;
    std::string repr;
    std::unordered_map<Direction, std::string> paths;
    std::unordered_map<Message, std::string> messages;

    std::vector<std::string> items;

    public:

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