#include <iostream>

int main() {
    int x = 10;
    int* ptr = &x;
    
    std::cout << x << std::endl; // print x normally
    std::cout << &x << std::endl; // print the address of x using &x
    std::cout << ptr << std::endl; // print what ptr contains (should be the address)
    std::cout << *ptr << std::endl; // print what ptr points to using *ptr
    *ptr = 99; // modify x through the pointer to 99
    std::cout << x << std::endl; // print x again to confirm it changed
}