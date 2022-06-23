#include "command.hpp"

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

Command::~Command() {
    
}

std::string Command::getRepr() {
    return std::string(repr);
}

std::vector<std::string> Command::getHints() {
    return std::vector<std::string>(hints);
}

bool Command::isMatch(std::string s) {
    return std::regex_match(s, pattern);
}

bool Command::tryEval(std::string s) {
    bool matches = isMatch(s);
    if (matches) {
        callback();
    }
    return matches;
}