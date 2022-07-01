#include "textbasedgame.hpp"

/* ------- TEXTBASEDGAME ------- */

TextBasedGame::TextBasedGame() {
    graphics = new Graphics();
    state = GameState::Loading;
}

TextBasedGame::~TextBasedGame() {
    delete graphics;
}

void TextBasedGame::Init() {

    currentRoom = "Kitchen";

    InitRooms();
    InitItems();
    InitCommands();

    state = GameState::Playing;
    Write(fmt::format("You are in the {}.", rooms.Get(currentRoom).GetRepr()));
}

void TextBasedGame::InitRooms() {

    /* creating */

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
        rooms.Add(room.GetName(), room);
    }

    /* linking */

    LinkRooms("Kitchen", Direction::North, "Bedroom"); // kitchen -> north -> bedroom
    std::cout<<fmt::format("'{}'\n", rooms.Get("Kitchen").GetPath(Direction::North));
}

void TextBasedGame::InitItems() {

    /* creating */

    for (auto &item : std::vector<Item>{
        Item("Red Key", "red key", std::unordered_map<Item::Message, std::string>{
            { Item::Message::OnInspect, "A shiny red key." }
        }),
        Item("Red Door", "red door", std::unordered_map<Item::Message, std::string>{
            { Item::Message::OnInspect, "This red door stands on the north side of the room, and it has a keyhole in the knob." }
        }, std::vector<Command>{
            Command("unlock (red )?door", {"unlock red door"}, [&]{
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
        items.Add(item.GetName(), item);
    }
    
    /* adding */

    AddItemToRoom("Red Key", "Kitchen");
    AddItemToRoom("Red Door", "Bedroom");
}

void TextBasedGame::InitCommands() {
    
    /* creating */

    /* movement */

    commands.Add("Move: North", Command("(go|move )?n(orth)?", { "go north", "move north", "north" }, [&]{ TryMove(Direction::North); }));
    commands.Add("Move: South", Command("(go|move )?s(outh)?", { "go south", "move south", "south" }, [&]{ TryMove(Direction::South); }));
    commands.Add("Move: East", Command("(go|move )?e(ast)?", { "go east", "move east", "east" }, [&]{ TryMove(Direction::East); }));
    commands.Add("Move: West", Command("(go|move )?w(est)?", { "go west", "move west", "west" }, [&]{ TryMove(Direction::West); }));
    commands.Add("Move: Unknown Direction", Command("(go|move ).*", {}, [&]{ Write(Messages::InvalidDir); }));

    /* inspection + visual */

    commands.Add("Get Current Room", Command("(where am i)|((current )?room)", { "current room", "room", "where am i" },
        [&]{ Write(fmt::format("You are in the {}.", rooms.Get(currentRoom).GetRepr())); })
    );
    commands.Add("Look Around", Command("look( around)?", {"look around"}, [&]{
        for (auto &item : rooms.Get(currentRoom).GetItems()) {
            items.Get(item).GetAttrs().isFound = true;
        }
        Write(rooms.Get(currentRoom).GetMessage(Room::Message::OnLook) + " " + CurrentRoomRepr());
    }));
    commands.Add("Check Inventory", Command("(check )?inv(entory)?", { "check inventory", "inventory" }, [&]{ Write(InventoryRepr()); }));

    /* take/drop items, special commands */

    for (auto& [name, item] : items) {
        auto repr = item.GetRepr();

        /* no hints versions of these are to be used when the player isn't supposed to be getting hints about it */

        commands.Add(fmt::format("Take Item: {}", name), Command(fmt::format("(take|pick up) {}", name), { fmt::format("take {}", repr), fmt::format("pick up {}", repr) }, [&]{ TryTakeItem(name); }));
        commands.Add(fmt::format("Take Item: {} (No Hints)", name), Command(fmt::format("(take|pick up) {}", name), {}, [&]{ TryTakeItem(name); }));

        commands.Add(fmt::format("Drop Item: {}", name), Command(fmt::format("(drop|put down) {}", name), { fmt::format("drop {}", repr), fmt::format("put down {}", repr) }, [&]{ TryDropItem(name); }));
        commands.Add(fmt::format("Drop Item: {} (No Hints)", name), Command(fmt::format("(drop|put down) {}", name), {}, [&]{ TryDropItem(name); }));

        commands.Add(fmt::format("Inspect Item: {}", name), Command(fmt::format("(look at|inspect) {}", name), { fmt::format("inspect {}", repr), fmt::format("look at {}", repr) }, [&]{ TryInspectItem(name); }));
        commands.Add(fmt::format("Inspect Item: {} (No Hints)", name), Command(fmt::format("(look at|inspect) {}", name), {}, [&]{ TryInspectItem(name); }));
    }

    /* take/drop failsafes */

    commands.Add("Take Item: Invalid", Command("take .*", {}, [&]{ Write(Messages::InvalidTake); }));
    commands.Add("Take Item: Unknown", Command("take.*", {}, [&]{ Write(Messages::UnknownTake); }));
    commands.Add("Drop Item: Invalid", Command("drop .*", {}, [&]{ Write(Messages::InvalidDrop); }));
    commands.Add("Drop Item: Unknown", Command("drop.*", {}, [&]{ Write(Messages::UnknownDrop); }));

    /* settings - text scroll speed */
    commands.Add("Set Text Scroll Speed: Slow", Command("set (textspeed|ts) (s(low)?)|(1)", { "set textspeed slow", "set ts slow" }, [&]{
        graphics->ChangeTextSpeed(Graphics::TextSpeed::Slow);
        Write(Messages::TextSpeedSet);
    }));
    commands.Add("Set Text Scroll Speed: Medium", Command("set (textspeed|ts) (m(ed(ium)?)?)|(2)", { "set textspeed med", "set ts med" }, [&]{
        graphics->ChangeTextSpeed(Graphics::TextSpeed::Medium);
        Write(Messages::TextSpeedSet);
    }));
    commands.Add("Set Text Scroll Speed: Fast", Command("set (textspeed|ts) (f(ast)?)|(3)", { "set textspeed fast", "set ts fast" }, [&]{
        graphics->ChangeTextSpeed(Graphics::TextSpeed::Fast);
        Write(Messages::TextSpeedSet);
    }));
    commands.Add("Set Text Scroll Speed: Invalid", Command("set (textspeed|ts).*", {}, [&]{ Write(Messages::InvalidTextSpeed); }));

    /* cursor style */
    commands.Add("Set Cursor Style: Vertical Bar", Command("set (cursor( style)?|cs|c) 1", {
        "set cursor style 1",
        "set cursor 1",
        "set cs 1"
    }, [&]{
        graphics->ChangeCursorStyle(Graphics::CursorStyle::VerticalBar);
        Write(Messages::CursorStyleSet);
    }));
    commands.Add("Set Cursor Style: Underline", Command("set (cursor( style)?|cs|c) 2", {
        "set cursor style 2",
        "set cursor 2",
        "set cs 2"
    }, [&]{
        graphics->ChangeCursorStyle(Graphics::CursorStyle::Underline);
        Write(Messages::CursorStyleSet);
    }));
    commands.Add("Set Cursor Style: OutlineBox", Command("set (cursor( style)?|cs|c) 3", {
        "set cursor style 3",
        "set cursor 3",
        "set cs 3"
    }, [&]{
        graphics->ChangeCursorStyle(Graphics::CursorStyle::OutlineBox);
        Write(Messages::CursorStyleSet);
    }));
    commands.Add("Set Cursor Style: TransparentBox", Command("set (cursor( style)?|cs|c) 4", {
        "set cursor style 4",
        "set cursor 4",
        "set cs 4"
    }, [&]{
        graphics->ChangeCursorStyle(Graphics::CursorStyle::TransparentBox);
        Write(Messages::CursorStyleSet);
    }));
    commands.Add("Set Cursor Style: Invalid", Command("set (cursor( style)?|cs|c).*", {}, [&]{
        Write(Messages::InvalidCursorStyle);
    }));
    
    /* misc. system */
    commands.Add("General Help", Command("help( me)?", {
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
    commands.Add("List Settings", Command("settings", { "settings", "game settings" }, [&]{
        Write(
            "set textspeed <slow/med/fast>\n"
            "set cursor <1/2/3/4>"
        );
    }));
    commands.Add("Exit Game", Command("(q(uit)?|exit)( game)?", { "exit game", "quit game" }, [&]{ ChangeState(GameState::ExitMenu); }));

    /* failsafes */
    commands.Add("Unknown Setting", Command("set.*", {}, [&]{ Write("What do you want to set?\nUsage: set <setting> <arg>"); }));
    commands.Add("Invalid Command", Command(".*", {}, [&]{ Write(Messages::InvalidCommand); }));

    commands.Add("Exit: Yes", Command("(y(es)?)|(exit)|(quit)", { "yes", "exit", "quit" }, []{ throw ExitGameException(); }));
    commands.Add("Exit: No", Command("n(o)?", { "no" }, [&]{ ChangeState(GameState::Playing); }));
    commands.Add("Exit: Unknown", Command(".*", {}, [&]{ Write(Messages::InvalidExitCommand); }));
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
    rooms.Get(a).SetPath(d, b);
    if (bothWays) {
        rooms.Get(b).SetPath(DirectionReverse(d), a);
    }
}

void TextBasedGame::AddItemToRoom(std::string itemName, std::string roomName) {
    rooms.Get(roomName).AddItem(itemName);
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
    switch(state) {
        case GameState::ExitMenu: {
            return std::vector<Command>{
                commands.Get("Exit: Yes"),
                commands.Get("Exit: No"),
                commands.Get("Exit: Unknown")
            };
        }

        case GameState::Loading: {
            return {};
        }

        case GameState::Playing: {

            std::vector<Command> cmds;

            /* movement */
            
            cmds.push_back(commands.Get("Move: North"));
            cmds.push_back(commands.Get("Move: South"));
            cmds.push_back(commands.Get("Move: East"));
            cmds.push_back(commands.Get("Move: West"));
            cmds.push_back(commands.Get("Move: Unknown Direction"));

            /* inspection/visual */

            cmds.push_back(commands.Get("Get Current Room"));
            cmds.push_back(commands.Get("Look Around"));
            cmds.push_back(commands.Get("Check Inventory"));

            /* take/drop items, special commands */

            for (auto& [name, item] : items) {
                auto hintText = item.GetAttrs().isFound? "" : " (No Hints)";
                bool inRoom = IsItemInRoom(name, currentRoom),
                    inInv = IsItemInInv(name);

                if (inRoom && item.GetFlags().canCarry) {
                    cmds.push_back(commands.Get(fmt::format("Take Item: {}{}", name, hintText)));
                }
                if (inInv) {
                    cmds.push_back(commands.Get(fmt::format("Drop Item: {}{}", name, hintText)));
                }
                if (inRoom) {
                    cmds.push_back(commands.Get(fmt::format("Inspect Item: {}{}", name, hintText)));
                }

                if (inInv || inRoom) {
                    for (const auto &cmd : item.GetSpecialCommands()) {
                        cmds.push_back(cmd);
                    }
                }
            }

            /* take/drop failsafes */
            cmds.push_back(commands.Get("Take Item: Invalid"));
            cmds.push_back(commands.Get("Take Item: Unknown"));
            cmds.push_back(commands.Get("Drop Item: Invalid"));
            cmds.push_back(commands.Get("Drop Item: Unknown"));

            /* settings - text scroll speed */
            cmds.push_back(commands.Get("Set Text Scroll Speed: Slow"));
            cmds.push_back(commands.Get("Set Text Scroll Speed: Medium"));
            cmds.push_back(commands.Get("Set Text Scroll Speed: Fast"));
            cmds.push_back(commands.Get("Set Text Scroll Speed: Invalid"));

            /* settings - cursor style */
            cmds.push_back(commands.Get("Set Cursor Style: Vertical Bar"));
            cmds.push_back(commands.Get("Set Cursor Style: Underline"));
            cmds.push_back(commands.Get("Set Cursor Style: OutlineBox"));
            cmds.push_back(commands.Get("Set Cursor Style: TransparentBox"));
            cmds.push_back(commands.Get("Set Cursor Style: Invalid"));
            
            /* misc. system */
            cmds.push_back(commands.Get("General Help"));
            cmds.push_back(commands.Get("List Settings"));
            cmds.push_back(commands.Get("Exit Game"));

            /* failsafes */
            cmds.push_back(commands.Get("Unknown Setting"));
            cmds.push_back(commands.Get("Invalid Command"));
            return cmds;
        }

        default: return {};
    }
}

/* player interaction */

void TextBasedGame::TryMove(Direction d) {
    std::string targetRoomName = rooms.Get(currentRoom).GetPath(d);
    
    /* if player cannot go that way */
    if (targetRoomName == "") {
        Write(Messages::BlockedDir);
    }
    /* if they can */
    else {
        currentRoom = targetRoomName;
        graphics->SetBackgroundImage(currentRoom);
        Write(fmt::format("You went {}.\n{}", DirectionRepr(d, false), rooms.Get(currentRoom).GetMessage(Room::Message::OnEnter)));
    }
}

void TextBasedGame::TryTakeItem(std::string itemName) {
    bool inInv = IsItemInInv(itemName);
    bool inRoom = IsItemInRoom(itemName, currentRoom);
    Item::Flags flags = items.Get(itemName).GetFlags();
    if (!inInv && inRoom && flags.canCarry) {
        player.AddItemToInv(itemName);
        rooms.Get(currentRoom).RemoveItem(itemName);
        items.Get(itemName).GetAttrs().isFound = true;
        Write(fmt::format("You took the {}.", items.Get(itemName).GetRepr()));
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
        rooms.Get(currentRoom).AddItem(itemName);
        Write(fmt::format("You dropped the {}.", items.Get(itemName).GetRepr()));
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
        Write(items.Get(itemName).GetMessage(Item::Message::OnInspect));
    } else {
        Write(Messages::InvalidInspect);
    }
}


bool TextBasedGame::IsItemInRoom(std::string itemName, std::string roomName) {
    auto items = rooms.Get(roomName).GetItems();
    return std::find(items.begin(), items.end(), itemName) != items.end();
}

bool TextBasedGame::IsItemInInv(std::string itemName) {
    auto inv = player.GetInventory();
    return std::find(inv.begin(), inv.end(), itemName) != inv.end();
}

std::string TextBasedGame::FullItemRepr(std::string itemName) {
    std::string repr = items.Get(itemName).GetRepr();
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
            std::stringstream ss("Your inventory contains ");
            for (uint8_t i = 0; i < inv.size() - 2; i++) {
                ss << (FullItemRepr(inv[i]) + ", ");
            }
            ss << FullItemRepr(inv[inv.size() - 2]);
            ss << fmt::format(" and {}.", FullItemRepr(inv[inv.size() - 1]));
            return ss.str();
        }
    }
}

std::string TextBasedGame::CurrentRoomRepr() {
    auto roomItems = rooms.Get(currentRoom).GetItems();
    switch(roomItems.size()) {
        case 0: return "There's nothing useful in here.";
        case 1: return fmt::format("You see {}.", FullItemRepr(roomItems[0]));
        case 2: return fmt::format("You see {} and {}.", FullItemRepr(roomItems[0]), FullItemRepr(roomItems[1]));
        case 3: return fmt::format("You see {}, {} and {}.", FullItemRepr(roomItems[0]), FullItemRepr(roomItems[1]), FullItemRepr(roomItems[2]));
        default: {
            std::stringstream ss("You see ");
            for (uint8_t i = 0; i < roomItems.size() - 2; i++) {
                ss << (FullItemRepr(roomItems[i]) + ", ");
            }
            ss << FullItemRepr(roomItems[roomItems.size() - 2]);
            ss << fmt::format(" and {}.", FullItemRepr(roomItems[roomItems.size() - 1]));
            return ss.str();
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
