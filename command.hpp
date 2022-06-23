#ifndef __COMMAND__
#define __COMMAND__

#include <functional>
#include <regex>
#include <string>

/*
    An in-game command. A list of these is matched against anything the player inputs.
    These are strictly internal and do not get shown to the player in any way, aside from the hints.
*/
class Command {

    private:

    /* internal name of the command */
    std::string repr;
    /* regex pattern object to match against */
    std::regex pattern;
    /*
        a list of hints that can be autofilled from what the player has
        typed, if part of it matches
    */
    std::vector<std::string> hints;
    /*
        the function to run when this is matched, must be void -> void
        every command must have one, and it can do nothing, but should
        print an error message at the very least
    */
    std::function<void()> callback;

    public:

    Command(
        std::string _repr,
        std::string _pattern,
        std::vector<std::string> _hints,
        std::function<void()> _callback = []{}
    );
    ~Command();

    std::string getRepr();
    std::vector<std::string> getHints();

    bool isMatch(std::string);
    bool tryEval(std::string);

};

#endif /* __COMMAND__ */