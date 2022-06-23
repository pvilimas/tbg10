#include "command.hpp"

/*
    Command constructor
    _repr - internal name of the command
    _pattern - regex string to match against - gets compiled into std::regex here with
        IGNORECASE flag and ^...$ for better matching
    _hints - a list of hints to show the player, potentially using these for autofill
    _callback - the callback function, must be void -> void
*/
Command::Command(
    std::string _repr,
    std::string _pattern,
    std::vector<std::string> _hints,
    std::function<void()> _callback
) {
    repr = _repr;
    pattern = std::regex("^" + _pattern + "$", std::regex::icase);
    hints = _hints;
    callback = _callback;
}

/*
    Command destructor, empty method
*/
Command::~Command() {

}

/* get internal name of the command */
std::string Command::getRepr() {
    return std::string(repr);
}

/* get a list of in-game hints */
std::vector<std::string> Command::getHints() {
    return std::vector<std::string>(hints);
}

/* does the command match this string? */
bool Command::isMatch(std::string s) {
    return std::regex_match(s, pattern);
}

/*
    try to match the string against this command
    if it matches call the callback and return true
    else return false
*/
bool Command::tryEval(std::string s) {
    bool matches = isMatch(s);
    if (matches) {
        callback();
    }
    return matches;
}