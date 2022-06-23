#include "textbasedgame.hpp"

/* ------- TEXTBASEDGAME ------- */

TextBasedGame::TextBasedGame() {
    graphics = new Graphics();
    state = GameState::Playing;

    /* init */

    /* ROOMS */
    for (auto &room : std::vector<Room>{
        Room("Kitchen", "kitchen", Room::MakeEmptyPaths(), std::unordered_map<Room::Message, std::string>{
            { Room::Message::OnEnter, "You have entered the kitchen." },
            { Room::Message::OnLook, "You are in the kitchen." }
        }),
        Room("Bedroom", "bedroom", Room::MakeEmptyPaths(), std::unordered_map<Room::Message, std::string>{
            { Room::Message::OnEnter, "You have entered the bedroom." },
            { Room::Message::OnLook, "You are in the bedroom." }
        }),
        Room("Garden", "garden", Room::MakeEmptyPaths(), std::unordered_map<Room::Message, std::string>{
            { Room::Message::OnEnter, "You have entered the garden." },
            { Room::Message::OnLook, "You are in the garden." }
        }),
    }) {
        rooms.emplace(room.getName(), room);
    }

    /* ITEMS */
    for (auto &item : std::vector<Item>{
        Item("Red Key", "red key", std::unordered_map<Item::Message, std::string>{
            { Item::Message::OnInspect, "A shiny red key." }
        }),
        Item("Red Door", "red door", std::unordered_map<Item::Message, std::string>{
            { Item::Message::OnInspect, "This red door stands on the north side of the room, and it has a keyhole in the knob." }
        }, std::vector<Command>{
            Command("Red Door: Unlock", "unlock (red )?door", std::vector<std::string>{"unlock red door"}, [&]{
                if (itemInInv("Red Key")) {
                    player.removeItemFromInv("Red Key");
                    linkRooms("Bedroom", Direction::North, "Garden");
                    write(std::vector<std::string>{
                        "You unlocked the red door.\n...",
                        "You can now go north.\n...",
                    });
                } else {
                    write("You don't have a key!");
                }
            })
        },
        Item::attrs(false),
        Item::flags(false)),
    }) {
        items.emplace(item.getName(), item);
    }
    
    /* setup */

    linkRooms("Kitchen", Direction::North, "Bedroom"); // kitchen -> north -> bedroom
    addItemToRoom("Red Key", "Kitchen");
    addItemToRoom("Red Door", "Bedroom");

    currentRoom = "Kitchen";
    write(fmt::format("You are in the {}.", rooms.at(currentRoom).getRepr()));
}

TextBasedGame::~TextBasedGame() {
    delete graphics;
}

void TextBasedGame::run() {
    static Timer backspaceTimer(0.1);
    while (!WindowShouldClose()) {
        // Get char pressed (unicode character) on the queue
        int key = GetCharPressed();

        // Check if more characters have been pressed on the same frame
        while (key > 0) {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125)) {
                graphics->addCharIn((char) key);
            }
            key = GetCharPressed();  // Check next character in the queue
        }

        if (IsKeyDown(KEY_BACKSPACE) && backspaceTimer.IntervalPassed()) {
            graphics->delCharIn();
        } else if (IsKeyPressed(KEY_BACKSPACE)) {
            graphics->delCharIn();
        } else if (IsKeyPressed(KEY_ENTER)) {
            if (!graphics->queueEmpty()) {
                graphics->dumpText();
            } else {
                eval(read());
            }
        } else if (IsKeyPressed(KEY_TAB)) {
            /* also clears hint */
            graphics->addHintToInput();
        }

        updateHint();
        graphics->draw();
    }
}

void TextBasedGame::changeState(TextBasedGame::GameState newState) {
    /* if user typed "exit" */
    if (state == GameState::Playing && newState == GameState::ExitMenu) {
        write(Messages::ExitConfirmation);
    }
    /* if user typed "exit" then "no" */
    else if (state == GameState::ExitMenu && newState == GameState::Playing) {
        write(fmt::format("You are in the {}.", currentRoom));
    }
    
    state = newState;
}

/* setup */

void TextBasedGame::linkRooms(std::string a, Direction d, std::string b, bool bothWays) {
    rooms.at(a).setPath(d, b);
    if (bothWays) {
        rooms.at(b).setPath(reverse(d), a);
    }
}

void TextBasedGame::addItemToRoom(std::string itemName, std::string roomName) {
    rooms.at(roomName).addItem(itemName);
}

void TextBasedGame::addItemToInventory(std::string itemName) {
    player.addItemToInv(itemName);
}

/* IO */

std::string TextBasedGame::read() {
    return graphics->getTextIn();
}

/* eval(read()) gets called when user hits enter */
void TextBasedGame::eval(std::string input) {
    clear();
    for (auto &cmd : getCommands()) {
        if (cmd.tryEval(input)) {
            break;
        }
    }

    graphics->setTextIn("");
}

void TextBasedGame::clear() {
    write("");
}

void TextBasedGame::write(std::string s) {
    /* split text into lines */
    static auto lineRegex = std::regex("(.{1,65})(?:(\\s)+|$|\n)");
    std::vector<std::string> res;

    int lc = 0;
    for (auto i = std::sregex_iterator(s.begin(), s.end(), lineRegex); i != std::sregex_iterator(); i++) {
        if (lc == Graphics::lineOutCount) {
            break;
        }
        res.push_back((*i).str());
        lc++;
    }

    /* pad it with empty lines */
    while (res.size() < Graphics::lineOutCount) {
        res.push_back("");
    }

    /* write all the lines */
    for (int i = 0; i < Graphics::lineOutCount; i++) {
        graphics->setTextOut(res.at(i), i);
    }
}

/* paginates output, ends with ..., waits for keypress, then displays next page */
void TextBasedGame::write(std::vector<std::string> v) {
    std::queue<std::string> q;
    for (auto &str : v) {
        q.push(str);
    }
    graphics->setTextIn("");
    while (!WindowShouldClose()) {
        if (GetKeyPressed() != KEY_NULL) {
            if (!graphics->queueEmpty()) {
                graphics->dumpText();
            } else {
                if (q.empty()) {
                    return;
                }
                /* advance */
                std::string s = q.front();
                q.pop();
                write(s);
            }
        }
        graphics->draw();
    }
}

void TextBasedGame::updateHint() {
    auto input = read();
    auto len = input.length();
    if (len == 0) {
        graphics->setHint("");
        return;
    }

    for (auto &cmd : getCommands()) {
        if (cmd.getHints().size() == 0) {
            continue;
        }

        for (auto &hint : cmd.getHints()) {
            if (hint.substr(0, len) == input) {
                graphics->setHint(hint.substr(len, std::string::npos));
                return;
            }
        }
    }
    graphics->setHint("");
}

/* commands */

std::vector<Command> TextBasedGame::getCommands() {
    std::vector<Command> cmds;
    
    if (state == GameState::Playing) {
        /* movement */
        cmds.push_back(Command("Move North", "(go|move )?n(orth)?", std::vector<std::string>{
            "go north",
            "move north",
            "north"
        }, [&]{ tryMove(Direction::North); }));
        cmds.push_back(Command("Move South", "(go|move )?s(outh)?", std::vector<std::string>{
            "go south",
            "move south",
            "south"
        }, [&]{ tryMove(Direction::South); }));
        cmds.push_back(Command("Move East", "(go|move )?e(ast)?", std::vector<std::string>{
            "go east",
            "move east",
            "east"
        }, [&]{ tryMove(Direction::East); }));
        cmds.push_back(Command("Move West", "(go|move )?w(est)?", std::vector<std::string>{
            "go west",
            "move west",
            "west"
        }, [&]{ tryMove(Direction::West); }));
        cmds.push_back(Command("Move Unknown Direction", "(go|move ).*", std::vector<std::string>(), [&]{ write(Messages::InvalidDir); }));

        /* inspection/visual */
        cmds.push_back(Command("Get Current Room", "(where am i)|((current )?room)", std::vector<std::string>{
            "current room",
            "room",
            "where am i"
        }, [&]{ write(fmt::format("You are in the {}.", rooms.at(currentRoom).getRepr())); }));
        cmds.push_back(Command("Look Around", "look( around)?", std::vector<std::string>{"look around"}, [&]{
            for (auto &item : rooms.at(currentRoom).getItems()) {
                items.at(item).getAttrs().isFound = true;
            }
            write(rooms.at(currentRoom).getMessage(Room::Message::OnLook) + " " + reprCurrentRoom());
        }));
        cmds.push_back(Command("Check Inventory", "(check )?inv(entory)?", std::vector<std::string>{
            "check inventory",
            "inventory"
        }, [&]{ write(reprInv()); }));

        /* take/drop items, special commands */

        for (auto& [name, item] : items) {
            auto repr = item.getRepr();

            std::vector<std::string> takeHints;
            std::vector<std::string> dropHints;
            std::vector<std::string> lookHints;
            if (item.getAttrs().isFound) {
                takeHints = std::vector<std::string>{
                    fmt::format("take {}", repr),
                    fmt::format("pick up {}", repr),
                };
                dropHints = std::vector<std::string>{
                    fmt::format("drop {}", repr),
                    fmt::format("put down {}", repr),
                };
                lookHints = std::vector<std::string>{
                    fmt::format("inspect {}", repr),
                    fmt::format("look at {}", repr),
                };
            } else {
                takeHints = {};
                dropHints = {};
                lookHints = {};
            }

            if (itemInRoom(name, currentRoom) && item.getFlags().canCarry) {
                cmds.push_back(Command(
                    fmt::format("Take Item: {}", name),
                    fmt::format("(take|pick up) {}", name),
                    takeHints,
                    [&]{ tryTakeItem(name); }
                ));
            }
            if (itemInInv(name)) {
                cmds.push_back(Command(
                    fmt::format("Drop Item: {}", name),
                    fmt::format("(drop|put down) {}", name),
                    dropHints,
                    [&]{ tryDropItem(name); }
                ));
            }
            if (itemInRoom(name, currentRoom)) {
                cmds.push_back(Command(
                    fmt::format("Inspect Item: {}", name),
                    fmt::format("(look at|inspect) {}", name),
                    lookHints,
                    [&]{ tryInspectItem(name); }
                ));
            }
            if (itemInInv(name) || itemInRoom(name, currentRoom)) {
                for (const auto &cmd : item.getSpecialCommands()) {
                    cmds.push_back(cmd);
                }
            }
        }

        /* take/drop failsafes */
        /* if hint is "" ignore */
        cmds.push_back(Command("Take Item: Invalid", "take .*", std::vector<std::string>(), [&]{ write(Messages::InvalidTake); }));
        cmds.push_back(Command("Take Item: Unknown", "take.*", std::vector<std::string>(), [&]{ write(Messages::UnknownTake); }));
        cmds.push_back(Command("Drop Item: Invalid", "drop .*", std::vector<std::string>(), [&]{ write(Messages::InvalidDrop); }));
        cmds.push_back(Command("Drop Item: Unknown", "drop.*", std::vector<std::string>(), [&]{ write(Messages::UnknownDrop); }));

        /* settings */

        /* text scroll speed */
        cmds.push_back(Command("Set Text Scroll Speed: Slow", "set (textspeed|ts) s(low)?", std::vector<std::string>{
            "set textspeed slow",
            "set ts slow"
        }, [&]{
            graphics->changeTextSpeed(Graphics::TextSpeed::Slow);
            write(Messages::TextSpeedSet);
        }));
        cmds.push_back(Command("Set Text Scroll Speed: Medium", "set (textspeed|ts) m(ed(ium)?)?", std::vector<std::string>{
            "set textspeed med",
            "set ts med"
        }, [&]{
            graphics->changeTextSpeed(Graphics::TextSpeed::Medium);
            write(Messages::TextSpeedSet);
        }));
        cmds.push_back(Command("Set Text Scroll Speed: Fast", "set (textspeed|ts) f(ast)?", std::vector<std::string>{
            "set textspeed fast",
            "set ts fast"
        }, [&]{
            graphics->changeTextSpeed(Graphics::TextSpeed::Fast);
            write(Messages::TextSpeedSet);
        }));
        cmds.push_back(Command("Set Text Scroll Speed: Invalid", "set (textspeed|ts).*", std::vector<std::string>(), [&]{
            write(Messages::InvalidTextSpeed);
        }));

        /* cursor style */
        cmds.push_back(Command("Set Cursor Style: Vertical Bar", "set (cursor( style)?|cs|c) 1", std::vector<std::string>{
            "set cursor style 1",
            "set cursor 1",
            "set cs 1"
        }, [&]{
            graphics->changeCursorStyle(Graphics::CursorStyle::VerticalBar);
            write(Messages::CursorStyleSet);
        }));
        cmds.push_back(Command("Set Cursor Style: Underline", "set (cursor( style)?|cs|c) 2", std::vector<std::string>{
            "set cursor style 2",
            "set cursor 2",
            "set cs 2"
        }, [&]{
            graphics->changeCursorStyle(Graphics::CursorStyle::Underline);
            write(Messages::CursorStyleSet);
        }));
        cmds.push_back(Command("Set Cursor Style: OutlineBox", "set (cursor( style)?|cs|c) 3", std::vector<std::string>{
            "set cursor style 3",
            "set cursor 3",
            "set cs 3"
        }, [&]{
            graphics->changeCursorStyle(Graphics::CursorStyle::OutlineBox);
            write(Messages::CursorStyleSet);
        }));
        cmds.push_back(Command("Set Cursor Style: TransparentBox", "set (cursor( style)?|cs|c) 4", std::vector<std::string>{
            "set cursor style 4",
            "set cursor 4",
            "set cs 4"
        }, [&]{
            graphics->changeCursorStyle(Graphics::CursorStyle::TransparentBox);
            write(Messages::CursorStyleSet);
        }));
        cmds.push_back(Command("Set Cursor Style: Invalid", "set (cursor( style)?|cs|c).*", std::vector<std::string>(), [&]{
            write(Messages::InvalidCursorStyle);
        }));
        
        /* misc. system */
        cmds.push_back(Command("General Help", "help( me)?", std::vector<std::string>{
            "help",
            "help me",
            "game help"
        }, [&]{
            write(std::vector<std::string>{
                "look around\n"
                "go <north/south/east/west>\n"
                "take/drop <item>\n"
                "...",

                "check inventory\n"
                "settings\n"
                "quit\n"
                "..."
            });
        }));
        cmds.push_back(Command("List Settings", "settings", std::vector<std::string>{
            "settings",
            "game settings"
        }, [&]{
            write(
                "set textspeed <slow/med/fast>\n"
                "set cursor <1/2/3/4>"
            );
        }));
        cmds.push_back(Command("Exit Game", "(q(uit)?|exit)( game)?", std::vector<std::string>{
            "exit game",
            "quit game"
        }, [&]{ changeState(GameState::ExitMenu); }));

        /* failsafes */
        cmds.push_back(Command("Unknown Setting", "set.*", std::vector<std::string>(), [&]{ write("What do you want to set?\nUsage: set <setting> <arg>"); }));
        cmds.push_back(Command("Invalid Command", ".*", std::vector<std::string>(), [&]{ write(Messages::InvalidCommand); }));

    } else if (state == GameState::ExitMenu) {
        cmds.push_back(Command("Exit: Yes", "(y(es)?)|(exit)|(quit)", std::vector<std::string>{
            "yes",
            "exit",
            "quit"
        }, []{ throw ExitGameException(); }));
        cmds.push_back(Command("Exit: No", "n(o)?", std::vector<std::string>{
            "no"
        }, [&]{ changeState(GameState::Playing); }));
        
        /* failsafe */
        cmds.push_back(Command("Exit: Unknown", ".*", std::vector<std::string>(), [&]{ write(Messages::InvalidExitCommand); }));
    }

    return cmds;
}

/* player interaction */

void TextBasedGame::tryMove(Direction d) {
    std::string targetRoomName = rooms.at(currentRoom).getPath(d);
    
    /* if player cannot go that way */
    if (targetRoomName == "") {
        write(Messages::BlockedDir);
    }
    /* if they can */
    else {
        currentRoom = rooms.at(currentRoom).getPath(d);
        graphics->setImage(currentRoom);
        write(fmt::format("You went {}.\n{}", repr(d), rooms.at(currentRoom).getMessage(Room::Message::OnEnter)));
    }
}

void TextBasedGame::tryTakeItem(std::string itemName) {
    bool inInv = itemInInv(itemName);
    bool inRoom = itemInRoom(itemName, currentRoom);
    Item::Flags flags = items.at(itemName).getFlags();
    if (!inInv && inRoom && flags.canCarry) {
        player.addItemToInv(itemName);
        rooms.at(currentRoom).removeItem(itemName);
        items.at(itemName).getAttrs().isFound = true;
        write(fmt::format("You took the {}.", items.at(itemName).getRepr()));
    } else if (inInv) {
        write(Messages::InvalidTakeHolding);
    } else if (!inRoom) {
        write(Messages::InvalidTake);
    } else if (!flags.canCarry) {
        write(Messages::CannotCarry);
    } else {
        write(Messages::UnknownError); // forgot edge case maybe 
    }
}

void TextBasedGame::tryDropItem(std::string itemName) {
    bool inInv = itemInInv(itemName);
    bool inRoom = itemInRoom(itemName, currentRoom);
    if (inInv && !inRoom) {
        player.removeItemFromInv(itemName);
        rooms.at(currentRoom).addItem(itemName);
        write(fmt::format("You dropped the {}.", items.at(itemName).getRepr()));
    } else if (!inInv) {
        write(Messages::InvalidDrop);
    } else if (inRoom) {
        write(Messages::InvalidDrop);
    } else {
        write(Messages::UnknownError); // forgot edge case maybe
    }
}


void TextBasedGame::tryInspectItem(std::string itemName) {
    bool inInv = itemInInv(itemName);
    bool inRoom = itemInRoom(itemName, currentRoom);

    if (inInv || inRoom) {
        write(items.at(itemName).getMessage(Item::Message::OnInspect));
    } else {
        write(Messages::InvalidInspect);
    }
}


bool TextBasedGame::itemInRoom(std::string itemName, std::string roomName) {
    auto items = rooms.at(roomName).getItems();
    return std::find(items.begin(), items.end(), itemName) != items.end();
}

bool TextBasedGame::itemInInv(std::string itemName) {
    auto inv = player.getInventory();
    return std::find(inv.begin(), inv.end(), itemName) != inv.end();
}

std::string TextBasedGame::reprItem(std::string itemName) {
    std::string repr = items.at(itemName).getRepr();
    for (char c : "aeiouAEIOU") {
        if (repr[0] == c) {
            return fmt::format("an {}", repr);
        }
    }
    return fmt::format("a {}", repr);
}

std::string TextBasedGame::reprInv() {
    auto inv = player.getInventory();
    switch(inv.size()) {
        case 0: return "Your inventory is empty.";
        case 1: return fmt::format("Your inventory contains {}.", reprItem(inv[0]));
        case 2: return fmt::format("Your inventory contains {} and {}.", reprItem(inv[0]), reprItem(inv[1]));
        case 3: return fmt::format("Your inventory contains {}, {} and {}.", reprItem(inv[0]), reprItem(inv[1]), reprItem(inv[2]));
        default: {
            std::string s = "Your inventory contains ";
            for (uint8_t i = 0; i < inv.size() - 2; i++) {
                s += (reprItem(inv[i]) + ", ");
            }
            s += reprItem(inv[inv.size() - 2]);
            s += fmt::format(" and {}.", reprItem(inv[inv.size() - 1]));
            return s;
        }
    }
}

std::string TextBasedGame::reprCurrentRoom() {
    auto roomItems = rooms.at(currentRoom).getItems();
    switch(roomItems.size()) {
        case 0: return "There's nothing useful in here.";
        case 1: return fmt::format("You see {}.", reprItem(roomItems[0]));
        case 2: return fmt::format("You see {} and {}.", reprItem(roomItems[0]), reprItem(roomItems[1]));
        case 3: return fmt::format("You see {}, {} and {}.", reprItem(roomItems[0]), reprItem(roomItems[1]), reprItem(roomItems[2]));
        default: {
            std::string s = "You see ";
            for (uint8_t i = 0; i < roomItems.size() - 2; i++) {
                s += (reprItem(roomItems[i]) + ", ");
            }
            s += reprItem(roomItems[roomItems.size() - 2]);
            s += fmt::format(" and {}.", reprItem(roomItems[roomItems.size() - 1]));
            return s;
        }
    }
}


/* ------- PLAYER ------- */

TextBasedGame::Player::Player() {
    inventory = std::vector<std::string>();
}

std::vector<std::string> TextBasedGame::Player::getInventory() {
    return std::vector<std::string>(inventory);
}

void TextBasedGame::Player::addItemToInv(std::string itemName) {
    inventory.push_back(itemName);
}

void TextBasedGame::Player::removeItemFromInv(std::string itemName) {
    inventory.erase(std::find(inventory.begin(), inventory.end(), itemName));
}
