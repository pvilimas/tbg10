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
        rooms.emplace(room.GetName(), room);
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
                if (IsItemInInv("Red Key")) {
                    player.RemoveItemFromInv("Red Key");
                    LinkRooms("Bedroom", Direction::North, "Garden");
                    Write(std::vector<std::string>{
                        "You unlocked the red door.\n...",
                        "You can now go north.\n...",
                    });
                } else {
                    Write("You don't have a key!");
                }
            })
        },
        Item::Attrs { false } ,
        Item::Flags { false } ),
    }) {
        items.emplace(item.GetName(), item);
    }
    
    /* setup */

    LinkRooms("Kitchen", Direction::North, "Bedroom"); // kitchen -> north -> bedroom
    AddItemToRoom("Red Key", "Kitchen");
    AddItemToRoom("Red Door", "Bedroom");

    currentRoom = "Kitchen";
    Write(fmt::format("You are in the {}.", rooms.at(currentRoom).GetRepr()));
}

TextBasedGame::~TextBasedGame() {
    delete graphics;
}

void TextBasedGame::Run() {
    static Timer backspaceTimer(0.1);
    while (!WindowShouldClose()) {
        /*  Get char pressed (unicode character) on the queue  */
        int key = GetCharPressed();

        /*  Check if more characters have been pressed on the same frame  */
        while (key > 0) {
            /*  NOTE: Only allow keys in range [32..125]  */
            if ((key >= 32) && (key <= 125)) {
                graphics->AddCharIn((char) key);
            }
            /*  Check next character in the queue  */
            key = GetCharPressed();
        }

        if (IsKeyDown(KEY_BACKSPACE) && backspaceTimer.IntervalPassed()) {
            graphics->DelCharIn();
        } else if (IsKeyPressed(KEY_BACKSPACE)) {
            graphics->DelCharIn();
        } else if (IsKeyPressed(KEY_ENTER)) {
            if (!graphics->IsQueueEmpty()) {
                graphics->DumpText();
            } else {
                Eval(Read());
            }
        } else if (IsKeyPressed(KEY_TAB)) {
            /*  also clears hint  */
            graphics->AddHintToInput();
        }

        UpdateHint();
        graphics->Draw();
    }
}

void TextBasedGame::ChangeState(TextBasedGame::GameState newState) {
    /* if user typed "exit" */
    if (state == GameState::Playing && newState == GameState::ExitMenu) {
        Write(Messages::ExitConfirmation);
    }
    /* if user typed "exit" then "no" */
    else if (state == GameState::ExitMenu && newState == GameState::Playing) {
        Write(fmt::format("You are in the {}.", currentRoom));
    }
    
    state = newState;
}

/* setup */

void TextBasedGame::LinkRooms(std::string a, Direction d, std::string b, bool bothWays) {
    rooms.at(a).SetPath(d, b);
    if (bothWays) {
        rooms.at(b).SetPath(ReverseDirection(d), a);
    }
}

void TextBasedGame::AddItemToRoom(std::string itemName, std::string roomName) {
    rooms.at(roomName).AddItem(itemName);
}

void TextBasedGame::AddItemToInventory(std::string itemName) {
    player.AddItemToInv(itemName);
}

/* IO */

std::string TextBasedGame::Read() {
    return graphics->GetTextIn();
}

/* Eval(Read()) gets called when user hits enter */
void TextBasedGame::Eval(std::string input) {
    Clear();
    for (auto &cmd : GetCommands()) {
        if (cmd.TryEval(input)) {
            break;
        }
    }

    graphics->SetTextIn("");
}

void TextBasedGame::Clear() {
    Write("");
}

void TextBasedGame::Write(std::string str) {
    /* split text into lines */
    static auto lineRegex = std::regex("(.{1,65})(?:(\\s)+|$|\n)");
    std::vector<std::string> res;

    int lc = 0;
    for (auto i = std::sregex_iterator(str.begin(), str.end(), lineRegex); i != std::sregex_iterator(); i++) {
        if (lc == Graphics::LineOutCount) {
            break;
        }
        res.push_back((*i).str());
        lc++;
    }

    /* pad it with empty lines */
    while (res.size() < Graphics::LineOutCount) {
        res.push_back("");
    }

    /* write all the lines */
    for (int i = 0; i < Graphics::LineOutCount; i++) {
        graphics->SetTextOut(res.at(i), i);
    }
}

void TextBasedGame::Write(std::vector<std::string> strs) {
    std::queue<std::string> q;
    for (auto &str : strs) {
        q.push(str);
    }
    graphics->SetTextIn("");
    while (!WindowShouldClose()) {
        if (GetKeyPressed() != KEY_NULL) {
            if (!graphics->IsQueueEmpty()) {
                graphics->DumpText();
            } else {
                if (q.empty()) {
                    return;
                }
                /* advance */
                std::string s = q.front();
                q.pop();
                Write(s);
            }
        }
        graphics->Draw();
    }
}

void TextBasedGame::UpdateHint() {
    std::string input = Read();
    auto len = input.length();
    if (len == 0) {
        graphics->SetHint("");
        return;
    }

    for (auto &cmd : GetCommands()) {
        if (cmd.GetHints().size() == 0) {
            continue;
        }

        for (auto &hint : cmd.GetHints()) {
            // return the first match found
            if (hint.substr(0, len) == input) {
                // npos = "go to the end of the string"
                graphics->SetHint(hint.substr(len, std::string::npos));
                return;
            }
        }
    }
    graphics->SetHint("");
}

/* commands */

std::vector<Command> TextBasedGame::GetCommands() {
    std::vector<Command> cmds;
    
    if (state == GameState::Playing) {
        /* movement */
        cmds.push_back(Command("Move North", "(go|move )?n(orth)?", std::vector<std::string>{
            "go north",
            "move north",
            "north"
        }, [&]{ TryMove(Direction::North); }));
        cmds.push_back(Command("Move South", "(go|move )?s(outh)?", std::vector<std::string>{
            "go south",
            "move south",
            "south"
        }, [&]{ TryMove(Direction::South); }));
        cmds.push_back(Command("Move East", "(go|move )?e(ast)?", std::vector<std::string>{
            "go east",
            "move east",
            "east"
        }, [&]{ TryMove(Direction::East); }));
        cmds.push_back(Command("Move West", "(go|move )?w(est)?", std::vector<std::string>{
            "go west",
            "move west",
            "west"
        }, [&]{ TryMove(Direction::West); }));
        cmds.push_back(Command("Move Unknown Direction", "(go|move ).*", std::vector<std::string>(), [&]{ Write(Messages::InvalidDir); }));

        /* inspection/visual */
        cmds.push_back(Command("Get Current Room", "(where am i)|((current )?room)", std::vector<std::string>{
            "current room",
            "room",
            "where am i"
        }, [&]{ Write(fmt::format("You are in the {}.", rooms.at(currentRoom).GetRepr())); }));
        cmds.push_back(Command("Look Around", "look( around)?", std::vector<std::string>{"look around"}, [&]{
            for (auto &item : rooms.at(currentRoom).GetItems()) {
                items.at(item).GetAttrs().isFound = true;
            }
            Write(rooms.at(currentRoom).GetMessage(Room::Message::OnLook) + " " + CurrentRoomRepr());
        }));
        cmds.push_back(Command("Check Inventory", "(check )?inv(entory)?", std::vector<std::string>{
            "check inventory",
            "inventory"
        }, [&]{ Write(InventoryRepr()); }));

        /* take/drop items, special commands */

        for (auto& [name, item] : items) {
            auto repr = item.GetRepr();

            std::vector<std::string> takeHints;
            std::vector<std::string> dropHints;
            std::vector<std::string> lookHints;
            if (item.GetAttrs().isFound) {
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

            if (IsItemInRoom(name, currentRoom) && item.GetFlags().canCarry) {
                cmds.push_back(Command(
                    fmt::format("Take Item: {}", name),
                    fmt::format("(take|pick up) {}", name),
                    takeHints,
                    [&]{ TryTakeItem(name); }
                ));
            }
            if (IsItemInInv(name)) {
                cmds.push_back(Command(
                    fmt::format("Drop Item: {}", name),
                    fmt::format("(drop|put down) {}", name),
                    dropHints,
                    [&]{ TryDropItem(name); }
                ));
            }
            if (IsItemInRoom(name, currentRoom)) {
                cmds.push_back(Command(
                    fmt::format("Inspect Item: {}", name),
                    fmt::format("(look at|inspect) {}", name),
                    lookHints,
                    [&]{ TryInspectItem(name); }
                ));
            }
            if (IsItemInInv(name) || IsItemInRoom(name, currentRoom)) {
                for (const auto &cmd : item.GetSpecialCommands()) {
                    cmds.push_back(cmd);
                }
            }
        }

        /* take/drop failsafes */
        /* if hint is "" ignore */
        cmds.push_back(Command("Take Item: Invalid", "take .*", std::vector<std::string>(), [&]{ Write(Messages::InvalidTake); }));
        cmds.push_back(Command("Take Item: Unknown", "take.*", std::vector<std::string>(), [&]{ Write(Messages::UnknownTake); }));
        cmds.push_back(Command("Drop Item: Invalid", "drop .*", std::vector<std::string>(), [&]{ Write(Messages::InvalidDrop); }));
        cmds.push_back(Command("Drop Item: Unknown", "drop.*", std::vector<std::string>(), [&]{ Write(Messages::UnknownDrop); }));

        /* settings */

        /* text scroll speed */
        cmds.push_back(Command("Set Text Scroll Speed: Slow", "set (textspeed|ts) (s(low)?)|(1)", std::vector<std::string>{
            "set textspeed slow",
            "set ts slow"
        }, [&]{
            graphics->ChangeTextSpeed(Graphics::TextSpeed::Slow);
            Write(Messages::TextSpeedSet);
        }));
        cmds.push_back(Command("Set Text Scroll Speed: Medium", "set (textspeed|ts) (m(ed(ium)?)?)|(2)", std::vector<std::string>{
            "set textspeed med",
            "set ts med"
        }, [&]{
            graphics->ChangeTextSpeed(Graphics::TextSpeed::Medium);
            Write(Messages::TextSpeedSet);
        }));
        cmds.push_back(Command("Set Text Scroll Speed: Fast", "set (textspeed|ts) (f(ast)?)|(3)", std::vector<std::string>{
            "set textspeed fast",
            "set ts fast"
        }, [&]{
            graphics->ChangeTextSpeed(Graphics::TextSpeed::Fast);
            Write(Messages::TextSpeedSet);
        }));
        cmds.push_back(Command("Set Text Scroll Speed: Invalid", "set (textspeed|ts).*", std::vector<std::string>(), [&]{
            Write(Messages::InvalidTextSpeed);
        }));

        /* cursor style */
        cmds.push_back(Command("Set Cursor Style: Vertical Bar", "set (cursor( style)?|cs|c) 1", std::vector<std::string>{
            "set cursor style 1",
            "set cursor 1",
            "set cs 1"
        }, [&]{
            graphics->ChangeCursorStyle(Graphics::CursorStyle::VerticalBar);
            Write(Messages::CursorStyleSet);
        }));
        cmds.push_back(Command("Set Cursor Style: Underline", "set (cursor( style)?|cs|c) 2", std::vector<std::string>{
            "set cursor style 2",
            "set cursor 2",
            "set cs 2"
        }, [&]{
            graphics->ChangeCursorStyle(Graphics::CursorStyle::Underline);
            Write(Messages::CursorStyleSet);
        }));
        cmds.push_back(Command("Set Cursor Style: OutlineBox", "set (cursor( style)?|cs|c) 3", std::vector<std::string>{
            "set cursor style 3",
            "set cursor 3",
            "set cs 3"
        }, [&]{
            graphics->ChangeCursorStyle(Graphics::CursorStyle::OutlineBox);
            Write(Messages::CursorStyleSet);
        }));
        cmds.push_back(Command("Set Cursor Style: TransparentBox", "set (cursor( style)?|cs|c) 4", std::vector<std::string>{
            "set cursor style 4",
            "set cursor 4",
            "set cs 4"
        }, [&]{
            graphics->ChangeCursorStyle(Graphics::CursorStyle::TransparentBox);
            Write(Messages::CursorStyleSet);
        }));
        cmds.push_back(Command("Set Cursor Style: Invalid", "set (cursor( style)?|cs|c).*", std::vector<std::string>(), [&]{
            Write(Messages::InvalidCursorStyle);
        }));
        
        /* misc. system */
        cmds.push_back(Command("General Help", "help( me)?", std::vector<std::string>{
            "help",
            "help me",
            "game help"
        }, [&]{
            Write(std::vector<std::string>{
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
            Write(
                "set textspeed <slow/med/fast>\n"
                "set cursor <1/2/3/4>"
            );
        }));
        cmds.push_back(Command("Exit Game", "(q(uit)?|exit)( game)?", std::vector<std::string>{
            "exit game",
            "quit game"
        }, [&]{ ChangeState(GameState::ExitMenu); }));

        /* failsafes */
        cmds.push_back(Command("Unknown Setting", "set.*", std::vector<std::string>(), [&]{ Write("What do you want to set?\nUsage: set <setting> <arg>"); }));
        cmds.push_back(Command("Invalid Command", ".*", std::vector<std::string>(), [&]{ Write(Messages::InvalidCommand); }));

    } else if (state == GameState::ExitMenu) {
        cmds.push_back(Command("Exit: Yes", "(y(es)?)|(exit)|(quit)", std::vector<std::string>{
            "yes",
            "exit",
            "quit"
        }, []{ throw ExitGameException(); }));
        cmds.push_back(Command("Exit: No", "n(o)?", std::vector<std::string>{
            "no"
        }, [&]{ ChangeState(GameState::Playing); }));
        
        /* failsafe */
        cmds.push_back(Command("Exit: Unknown", ".*", std::vector<std::string>(), [&]{ Write(Messages::InvalidExitCommand); }));
    }

    return cmds;
}

/* player interaction */

void TextBasedGame::TryMove(Direction d) {
    std::string targetRoomName = rooms.at(currentRoom).GetPath(d);
    
    /* if player cannot go that way */
    if (targetRoomName == "") {
        Write(Messages::BlockedDir);
    }
    /* if they can */
    else {
        currentRoom = rooms.at(currentRoom).GetPath(d);
        graphics->SetBackgroundImage(currentRoom);
        Write(fmt::format("You went {}.\n{}", ReprDirection(d), rooms.at(currentRoom).GetMessage(Room::Message::OnEnter)));
    }
}

void TextBasedGame::TryTakeItem(std::string itemName) {
    bool inInv = IsItemInInv(itemName);
    bool inRoom = IsItemInRoom(itemName, currentRoom);
    Item::Flags flags = items.at(itemName).GetFlags();
    if (!inInv && inRoom && flags.canCarry) {
        player.AddItemToInv(itemName);
        rooms.at(currentRoom).RemoveItem(itemName);
        items.at(itemName).GetAttrs().isFound = true;
        Write(fmt::format("You took the {}.", items.at(itemName).GetRepr()));
    } else if (inInv) {
        Write(Messages::InvalidTakeHolding);
    } else if (!inRoom) {
        Write(Messages::InvalidTake);
    } else if (!flags.canCarry) {
        Write(Messages::CannotCarry);
    } else {
        Write(Messages::UnknownError); // forgot edge case maybe 
    }
}

void TextBasedGame::TryDropItem(std::string itemName) {
    bool inInv = IsItemInInv(itemName);
    bool inRoom = IsItemInRoom(itemName, currentRoom);
    if (inInv && !inRoom) {
        player.RemoveItemFromInv(itemName);
        rooms.at(currentRoom).AddItem(itemName);
        Write(fmt::format("You dropped the {}.", items.at(itemName).GetRepr()));
    } else if (!inInv) {
        Write(Messages::InvalidDrop);
    } else if (inRoom) {
        Write(Messages::InvalidDrop);
    } else {
        Write(Messages::UnknownError); // forgot edge case maybe
    }
}


void TextBasedGame::TryInspectItem(std::string itemName) {
    bool inInv = IsItemInInv(itemName);
    bool inRoom = IsItemInRoom(itemName, currentRoom);

    if (inInv || inRoom) {
        Write(items.at(itemName).GetMessage(Item::Message::OnInspect));
    } else {
        Write(Messages::InvalidInspect);
    }
}


bool TextBasedGame::IsItemInRoom(std::string itemName, std::string roomName) {
    auto items = rooms.at(roomName).GetItems();
    return std::find(items.begin(), items.end(), itemName) != items.end();
}

bool TextBasedGame::IsItemInInv(std::string itemName) {
    auto inv = player.GetInventory();
    return std::find(inv.begin(), inv.end(), itemName) != inv.end();
}

std::string TextBasedGame::FullItemRepr(std::string itemName) {
    std::string repr = items.at(itemName).GetRepr();
    for (char c : "aeiou") {
        if (repr[0] == c || repr[0] == c - 32) {
            return fmt::format("an {}", repr);
        }
    }
    return fmt::format("a {}", repr);
}

std::string TextBasedGame::InventoryRepr() {
    auto inv = player.GetInventory();
    switch(inv.size()) {
        case 0: return "Your inventory is empty.";
        case 1: return fmt::format("Your inventory contains {}.", FullItemRepr(inv[0]));
        case 2: return fmt::format("Your inventory contains {} and {}.", FullItemRepr(inv[0]), FullItemRepr(inv[1]));
        case 3: return fmt::format("Your inventory contains {}, {} and {}.", FullItemRepr(inv[0]), FullItemRepr(inv[1]), FullItemRepr(inv[2]));
        default: {
            std::string s = "Your inventory contains ";
            for (uint8_t i = 0; i < inv.size() - 2; i++) {
                s += (FullItemRepr(inv[i]) + ", ");
            }
            s += FullItemRepr(inv[inv.size() - 2]);
            s += fmt::format(" and {}.", FullItemRepr(inv[inv.size() - 1]));
            return s;
        }
    }
}

std::string TextBasedGame::CurrentRoomRepr() {
    auto roomItems = rooms.at(currentRoom).GetItems();
    switch(roomItems.size()) {
        case 0: return "There's nothing useful in here.";
        case 1: return fmt::format("You see {}.", FullItemRepr(roomItems[0]));
        case 2: return fmt::format("You see {} and {}.", FullItemRepr(roomItems[0]), FullItemRepr(roomItems[1]));
        case 3: return fmt::format("You see {}, {} and {}.", FullItemRepr(roomItems[0]), FullItemRepr(roomItems[1]), FullItemRepr(roomItems[2]));
        default: {
            std::string s = "You see ";
            for (uint8_t i = 0; i < roomItems.size() - 2; i++) {
                s += (FullItemRepr(roomItems[i]) + ", ");
            }
            s += FullItemRepr(roomItems[roomItems.size() - 2]);
            s += fmt::format(" and {}.", FullItemRepr(roomItems[roomItems.size() - 1]));
            return s;
        }
    }
}


/* ------- PLAYER ------- */

TextBasedGame::Player::Player() {
    inventory = std::vector<std::string>();
}

std::vector<std::string>& TextBasedGame::Player::GetInventory() {
    return inventory;
}

void TextBasedGame::Player::AddItemToInv(std::string itemName) {
    inventory.push_back(itemName);
}

void TextBasedGame::Player::RemoveItemFromInv(std::string itemName) {
    inventory.erase(std::find(inventory.begin(), inventory.end(), itemName));
}
