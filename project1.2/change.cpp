#include <iostream>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string str;
    while(std::cin >> str) {
        if(str.size() == 16) {
            std::cout << ".half 0b" << str << std::endl;
        } else {
            std::cout << ".word 0b" << str << std::endl;
        }
    }
    return 0;
}