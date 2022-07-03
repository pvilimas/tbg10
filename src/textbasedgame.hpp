#ifndef __TEXTBASEDGAME__
#define __TEXTBASEDGAME__

#include <algorithm>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include "collection.hpp"
#include "command.hpp"
#include "item.hpp"
#include "room.hpp"
#include "graphics.hpp"
#include "timer.hpp"

/*

    TextBasedGame class - runs the game, has a graphics instance
    main.cpp is used to start up this class, catch a couple of exceptions, wrap it up and that's it
    nearly all work is done inside here, including:
    - game loop
    - initializing everything
        - TODO find a file format or something to load from lol (much later)
    - reading user input and evaluating to commands
    - manipulating inventory, room.items, rooms, [npc dialog options]
    - sending messages and prompts to the player
    - all moving around rooms, taking or dropping items, [talking to npcs]
    - drawing everything to the screen

    TODO pressing up/down cycles through history (a fun one)
    TODO have a thing that maps from currentGameState to a function, to automate repeated prompts:
    write(fmt::format("You are in the {}.", currentRoom));

    TODO much much later - convert to ABC, several base classes for different games

*/

class TextBasedGame {
    
    public:

    /*
        What state the game is currently in - "where" is the player?
        Has different commands available for each level

        TODO test for bugs like maybe keymashing as it's loading the game, might segfault idk
    */
    enum GameState {
        /*
            not initialized yet or in the process of reloading something 
        */
        Loading,
        /*
            normal state of gameplay, player is in this 99% of the time
            this state doesn't need any init logic in SetState()
        */
        Playing,
        /*
            when player said quit/exit - this state forms a loop
            the only commands are:
            y/yes -> quits
            n/no -> return to GameState::Playing
            anything else -> ignores and repeats prompt
        */
        ExitMenu
    };

    /*
        when the user quits the game, this gets thrown to go back to main() and unload everything
        TODO add StopTalkingToNPCException, maybe (prob later)
    */
    class ExitGameException {};
    
    struct Messages {
        /*
            Confirmations, status messages
        */

        /*  Printed automatically every game loop during exit menu  */
        static inline std::string ExitConfirmation = "Do you want to exit? (y/n)";
        /*  When player updates text speed to any value (even the same)  */
        static inline std::string TextSpeedSet = "Text speed updated.";
        /*  When player updates cursor style to any value (even the same)  */
        static inline std::string CursorStyleSet = "Cursor style updated.";

        /*
            Errors
            
            generally, 
            "Invalid..." = invalid argument
            "Unknown..." = missing argument

            no need for Errors substruct, it's pretty obvious which ones are errors (for now anyway)
        */
        
        /*  When the player tries to move north (or whatever) and there's no path that way  */
        static inline std::string BlockedDir = "You can't go that way.";
        /*
            "move asidhsadjka"
            "move"
        */
        static inline std::string InvalidDir = "Which way?";
        /*  When the player tries to inspect an item they aren't holding + not in the current room  */
        static inline std::string InvalidInspect = "You don't see that in here.";
        /*  When the player tries to take an item not in the current room  */
        static inline std::string InvalidTake = "You don't see that in here.";
        /*  When the player tries to take an item they are holding already  */
        static inline std::string InvalidTakeHolding = "You already have that.";
        /*
            "take asidhsadjka"
            "take"
        */
        static inline std::string UnknownTake = "What do you want to take?";
        /*  When the player tries to drop an item they aren't holding  */
        static inline std::string InvalidDrop = "You're not holding that item.";
        /*  "drop"  */
        static inline std::string UnknownDrop = "What do you want to drop?";
        /*  "carry 900lb elephant corpse"  */
        static inline std::string CannotCarry = "You don't need that.";
        /*
            "set textspeed veryfast"
            "set textspeed"
        */
        static inline std::string InvalidTextSpeed = "Usage: set textspeed <slow/med/fast>";
        /*
            "set cursor 11"
            "set cursor"
        */
        static inline std::string InvalidCursorStyle = "Usage: set cursor <1/2/3/4>";
        /*  "asjkdgasdfad"  */
        static inline std::string InvalidCommand = "Command not recognized.";
        /*  "asjkdgasdfad" on the quit menu  */
        static inline std::string InvalidExitCommand = "Command not recognized. Do you want to exit? (y/n)";
        /*  hopefully i will never see this  */
        static inline std::string UnknownError = "ERROR_SELF_DESTRUCTING_IN_5_SEC";
    };

    static inline std::string StartingRoom = "Kitchen";

    /*
        Everything to do with the player:
        - inventory stored here
        - methods to get + add/remove from
    */
    class Player {
        private:

        /*  list of names of all items in the player's inventory  */
        std::vector<std::string> inventory;

        public:
        
        Player();

        /*  returns a list of names of all items in the player's inventory  */
        std::vector<std::string>& GetInventory();

        /*  these methods assume item is or is not in inv already  */

        void AddItemToInv(std::string itemName);
        void RemoveItemFromInv(std::string itemName);

    };

    private:

    /*  the graphics instance  */
    Graphics *graphics;

    /*  the current state of the game (see GameState)  */
    GameState state;

    Collection<Command> commands;
    Collection<Item> items;
    Collection<Room> rooms;

    /*
        name of the room the player is currently in
        common idiom for this is rooms.at(currentRoom).doWhatever()
    */
    std::string currentRoom;
    
    /*  the player object  */
    Player player;

    public:

    /*  case-insensitive string compare - returns true if a == b  */
    static bool StrEq(const std::string& a, const std::string& b);

    /*
        TextBasedGame constructor - initializeaz:
        - graphics instance
        - gamestate - playing
        - all rooms
        - all items
        and:
        - links rooms
        - adds items to rooms
        - TODO adds npcs to rooms
        - sets current room
        - writes starting message (You are in the ...)
    */
    TextBasedGame();

    /*
        TextBasedGame destructor
        pretty much just deletes the graphics instance
    */
    ~TextBasedGame();

    void Init();
    void InitCommands();
    void InitRooms();
    void InitItems();

    /*
        Runs the entire game loop
        Manages key inputs:
        - chars pressed on kb
        - sends them to graphics to add/del chars from textIn
        - eval if ENTER is pressed and there's text in the box
        - dump if any key is pressed and there's text in the queue
        - add hint if TAB is pressed and there's a hint
        and draws graphics
    */
    void Run();
    /*
        change the gamestate
        contains initialization logic (and status messages) based on old state and new state
    */
    void ChangeState(GameState newState);

    /* setup */

    /*
        Links rooms A and B (those are the names) in the given direction with an optional argument for going both ways
        bothways = false:
            A -dir-> B
        bothways = true:
            A -dir-> B
            B -rev-> A
        where rev is the reverse of dir
    */
    void LinkRooms(std::string a, Direction d, std::string b, bool bothWays = true);

    /*  adds an item to a room's item list, given their names  */
    void AddItemToRoom(std::string itemName, std::string roomName);
   
    /*  adds an item to the player's inventory (mainly used for setup like addItemToRoom is, i think)  */
    void AddItemToInventory(std::string itemName);

    /*  IO functions  */

    /*
        returns whatever the player has currently typed, does not change it
        (should be) only ever used with TBG::eval()
    */
    std::string Read();

    /*
        evaluates the given string, passing it through a list of all commands (from TBG::getCommands())
        (should be) only ever used with TBG::read()
    */
    void Eval(std::string input);
    
    /*  literally just a call to TBG::write(""), clears everything  */
    void Clear();
    
    /*
        writes a string to textOut, splits it into lines
        only writes 4 lines (or however many are on the screen)
    */
    void Write(std::string str);

    /*
        writes a list of strings to textOut - has an internal loop that goes until it's done
        paginates output, ends with ..., waits for keypress, then displays next page
        the ... must be added manually (for now) (TODO)
    */
    void Write(std::vector<std::string> strs);

    /*  looks through the hints of every command in getCommands and the first one to match is displayed  */
    void UpdateHint();

    /*
        NOTE: paginate is too much work rn, but the old code is on discord
        so for now manually check every message and add ... where necessary
        will def be needed with a variable # of lines
    */

    /* commands */

    /*
        returns a list of all currently available commands - these depend on:
        - the current GameState - quitmenu has its own set of commands (y/n/.*)
        - what room the player is in - items will be available to grab if they're there
        - what the player has in their inventory - same as above, with dropping items
        - other stuff, probably
    */
    std::vector<Command> GetCommands();

    /* player interaction */

    /*
        try to move in the given direction, print either "you went xyz" or Messages::BlockedDir if
        you can't go that way
    */
    void TryMove(Direction d);

    /*
        try to take the given item, either print that you took it, or one of several errors:
        - InvalidTakeHolding if you're already holding it
        - InvalidTake if it's not in the current room
        - CannotCarry if the carry flag (lol) isn't set
        - UnknownError if something unexpected happens
    */
    void TryTakeItem(std::string itemName);

    /*
        try to drop the given item, either print successful drop msg or:
        - InvalidDrop if you aren't carrying it
        - InvalidDrop again if it's in the current room (trust me this makes sense) - no dupes allowed
            - BUG here? something with dupe items in the current room and inventory
            - ez fix is don't allow any dupes anywhere
        - UnknownError if something else happens
    */
    void TryDropItem(std::string itemName);

    /*
        try to inspect the item, if you can then print the msg, otherwise:
        - InvalidInspect if it's nowhere to be seen (inv or currentroom)
    */
    void TryInspectItem(std::string itemName);

    /*  sets currentroom and graphics.bgimage  */
    void SetCurrentRoom(std::string newRoomName);

    /*  is this item in this room?  */
    bool IsItemInRoom(std::string itemName, std::string roomName);
    /*  is this item in the player's inventory?  */
    bool IsItemInInv(std::string itemName);

    /*
        not to be confused with Item::getRepr(), which this calls
        returns "a/an itemName" type format:
        - a lamp
        - an orange
        - some shoes
        TODO plurals, like some ^
    */
    std::string FullItemRepr(std::string itemName);

    /*
        gets a string representation of the player's inventory - cases below:
        - 0 items -> Your inventory is empty.
        - 1 item -> Your inventory contains <a lamp>.
        - 2 items -> Your inventory contains <a lamp> and <an orange>.
        - 3 items -> Your inventory contains <a lamp>, <an orange>, and <some jews>.
        - 4+ items -> Your inventory contains <a lamp>, <an orange>, ..., and <some jews>.
    */
    std::string InventoryRepr();
    
    /*
        gets a string representation of the list of items in the current room - cases:
        - 0 items -> There's nothing useful in here.
        - 1 item -> You see <a lamp>.
        - 2 items -> You see <a lamp> and <an orange>.
        - 3 items -> You see <a lamp>, <an orange>, and <some jews>.
        - 4+ items -> You see <a lamp>, <an orange>, ..., and <some jews>.
    */
    std::string CurrentRoomRepr();

};

#endif /* __TEXTBASEDGAME__ */