#ifndef __COLLECTION__
#define __COLLECTION__

#include <string>
#include <unordered_map>

#include "command.hpp"
#include "item.hpp"
#include "room.hpp"

/*  wrapper around unordered_map<T> with string keys  */
template<class T>
class Collection {
    private:

    std::unordered_map<std::string, T> items;

    public:

    Collection();
    void Add(std::string name, T item);
    T& Get(std::string name);
    void Delete(std::string name);

    /* iterator stuff */

    auto inline begin() {
        return items.begin();
    }
    auto inline end() {
        return items.end();
    }
    auto inline cbegin() const {
        return items.cbegin();
    }
    auto inline cend() const {
        return items.cend();
    }
};

#endif /* __COLLECTION__ */