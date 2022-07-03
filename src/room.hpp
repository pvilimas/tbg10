#ifndef __ROOM__
#define __ROOM__

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "globals.hpp"

/*

*/
class Room {

    public:

    /*
        TODO add OnFirstEnter, and:
            - add room.playerHasVisited
            - keep track of that in game
            
        a type of room message, used to pass messages by type
    */
    enum Message {
        /* ex. you have entered the kitchen. */
        OnEnter,
        /* ex. you are in the kitchen. there is a stove. */
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
    static std::unordered_map<Direction, std::string> MakeEmptyPaths();

    /*
        Room constructor - example call:
        Room("Kitchen", "kitchen", Room::MakeEmptyPaths(), std::unordered_map<Room::Message, std::string>{
            { Room::Message::OnEnter, "You have entered the kitchen." },
            { Room::Message::OnLook, "You are in the kitchen." }
        })
    */
    Room(
        std::string _name,
        std::string _repr,
        std::unordered_map<Direction, std::string> _paths,
        std::unordered_map<Message, std::string> _messages
    );

    /*  Room destructor, empty method  */
    ~Room();

    /*  gets the internal name of the room  */
    std::string& GetName();

    /*  gets the in-game string representation of the room  */
    std::string& GetRepr();

    /*  gets a specific message type  */
    std::string& GetMessage(Message mtype);

    /*
        gets the name of the room in that direction from this room
        null value = empty string
    */
    std::string& GetPath(Direction d);
    
    /*
        sets the name of the room in that direction from this room
        null value = empty string
    */
    void SetPath(Direction d, std::string other);

    
    /*  get the list of names for every item in the room  */
    std::vector<std::string>& GetItems();
    
    /*  adds an item's name to the list of items in the room  */
    void AddItem(std::string itemName);
    
    /*  removes an item's name from the list of items in the room  */
    void RemoveItem(std::string itemName);

};

#endif /* __ROOM__ */