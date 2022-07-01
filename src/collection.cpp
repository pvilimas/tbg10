#include "collection.hpp"

template<class T>
Collection<T>::Collection() { }

template<class T>
void Collection<T>::Add(std::string name, T item) {
    items.emplace(name, item);
}

template<class T>
T& Collection<T>::Get(std::string name) {
    return items.at(name);
}

template<class T>
void Collection<T>::Delete(std::string name) {
    items.erase(name);
}

template class Collection<Command>;
template class Collection<Item>;
template class Collection<Room>;