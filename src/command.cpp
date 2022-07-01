#include "command.hpp"

Command::Command(
    std::string _pattern_str,
    std::vector<std::string> _hints,
    std::function<void()> _callback
) {
    pattern = std::regex("^" + _pattern_str + "$", std::regex::icase);
    hints = _hints;
    callback = _callback;
}

Command::~Command() {

}

std::vector<std::string>& Command::GetHints() {
    return hints;
}

bool Command::IsMatch(std::string& s) {
    return std::regex_match(s, pattern);
}

bool Command::TryEval(std::string& s) {
    bool is_match = IsMatch(s);
    if (is_match) {
        callback();
    }
    return is_match;
}