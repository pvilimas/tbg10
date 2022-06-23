#ifndef __COMMAND__
#define __COMMAND__

#include <functional>
#include <regex>
#include <string>

class Command {

    private:

    std::string repr;
    std::regex pattern;
    std::vector<std::string> hints;
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