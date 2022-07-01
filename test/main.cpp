#include <iostream>
#include "template.hpp"

int main() {
    Template<int> i(5);
    Template<char> c('b');
    Template<float> f(6.7);
    std::cout << i.getData() << c.getData() << f.getData() << std::endl;
}