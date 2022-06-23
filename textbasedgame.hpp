#ifndef __TEXTBASEDGAME__
#define __TEXTBASEDGAME__

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "command.hpp"
#include "item.hpp"
#include "room.hpp"
#include "graphics.hpp"
#include "timer.hpp"

/* convert to ABC later */
class TextBasedGame {
    
    public:

    enum GameState { Playing, ExitMenu };

    class ExitGameException {};
    
    struct Messages {
        static inline std::string ExitConfirmation = "Do you want to exit? (y/n)";
        static inline std::string TextSpeedSet = "Text speed updated.";
        static inline std::string CursorStyleSet = "Cursor style updated.";

        /*
            errors
            invalid = invalid arg
            unknown = missing arg
        */
        static inline std::string BlockedDir = "You can't go that way.";
        static inline std::string InvalidDir = "Which way?";
        static inline std::string InvalidInspect = "You don't see that in here.";
        static inline std::string InvalidTake = "You don't see that in here.";
        static inline std::string InvalidTakeHolding = "You already have that.";
        static inline std::string UnknownTake = "What do you want to take?";
        static inline std::string InvalidDrop = "You're not holding that item.";
        static inline std::string UnknownDrop = "What do you want to drop?";
        static inline std::string CannotCarry = "You don't need that.";
        static inline std::string InvalidTextSpeed = "Usage: set textspeed <slow/med/fast>";
        static inline std::string InvalidCursorStyle = "Usage: set cursor <1/2/3/4>";
        static inline std::string InvalidCommand = "Command not recognized.";
        static inline std::string InvalidExitCommand = "Command not recognized. Do you want to exit? (y/n)";
        static inline std::string UnknownError = "ERROR_SELF_DESTRUCTING_IN_5_SEC";


    };

    class Player {
        private:

        std::vector<std::string> inventory;

        public:
        
        Player();

        std::vector<std::string> getInventory();

        /* these methods assume item is or is not in inv already */

        void addItemToInv(std::string itemName);
        void removeItemFromInv(std::string itemName);

    };

    private:

    Graphics *graphics;
    GameState state;

    /* NO DUPES duh */
    std::unordered_map<std::string, Room> rooms;
    std::unordered_map<std::string, Item> items;
    std::string currentRoom;
    
    Player player;

    public:

    TextBasedGame();
    ~TextBasedGame();

    void run();
    void changeState(GameState newState);

    /* setup */

    void linkRooms(std::string a, Direction d, std::string b, bool bothWays = true);
    void addItemToRoom(std::string itemName, std::string roomName);
    void addItemToInventory(std::string itemName);

    /* IO */

    std::string read();
    void eval(std::string);
    void clear();
    void write(std::string);
    void write(std::vector<std::string>);
    void updateHint();

    /*
        NOTE: paginate is too much work rn, but the old code is on discord
        so for now manually check every message and add ... where necessary
        will def be needed with a variable # of lines
    */

    /* commands */

    std::vector<Command> getCommands();

    /* player interaction */

    void tryMove(Direction);
    void tryTakeItem(std::string);
    void tryDropItem(std::string);
    void tryInspectItem(std::string);

    bool itemInRoom(std::string itemName, std::string roomName);
    bool itemInInv(std::string itemName);

    // "a/an itemName"
    std::string reprItem(std::string itemName);
    std::string reprInv();
    std::string reprCurrentRoom();

};

#endif /* __TEXTBASEDGAME__ */