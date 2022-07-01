#include "template.hpp"

template<class T>
Template<T>::Template(T _data) {
    data = _data;
}

template<class T>
T Template<T>::getData() {
    return data;
}

template class Template<int>;
template class Template<char>;
template class Template<float>;