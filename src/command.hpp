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

    /*  internal name of the command  */
    std::string repr;
    /*  regex pattern object to match against  */
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

    /*
        Command constructor
        _repr - internal name of the command
        _pattern_str - regex string to match against - gets compiled into std::regex here with
            IGNORECASE flag and ^...$ for better matching
        _hints - a list of hints to show the player, potentially using these for autofill
        _callback - the callback function, must be void -> void
    */
    Command(
        std::string _repr,
        std::string _pattern_str,
        std::vector<std::string> _hints,
        std::function<void()> _callback = []{}
    );
    /*  Command destructor, empty method  */
    ~Command();

    /*  get internal name of the command  */
    std::string& GetRepr();
    
    /*  get a list of in-game hints  */
    std::vector<std::string>& GetHints();

    /*  does the command match this string?  */
    bool IsMatch(std::string& s);

    /*
        try to match the string against this command
        if it matches call the callback and return true
        else return false
    */
    bool TryEval(std::string& s);

};

#endif /* __COMMAND__ */