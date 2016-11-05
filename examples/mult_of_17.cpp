#include <iostream>
#include <vector>
#include "../Streams.h"

int main() {
    std::vector<int> vec { 0, 17, 18, 34, 35, 36, 37, 170};

    int sum = streams::from(vec) // you can use a 'stream' created before
        .filter([](auto& e) {return e % 17 == 0; })
        .map([](auto& e) { return e*e; })
        .fold(0, std::plus<int>{});
    std::cout << sum;
}