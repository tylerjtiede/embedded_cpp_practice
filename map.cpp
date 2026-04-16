#include <iostream>
#include <unordered_map>
#include <string>

std::unordered_map<std::string, std::string> phone_book;

std::string lookup(std::string name) {
    if (phone_book.count(name)) {
        return phone_book[name];
    }
    std::cout << name << " not found" << std::endl;
    return "";
}

void remove_contact(std::string name) {
    phone_book.erase(name);
}

void print_all_contacts() {
    for (const auto& pair : phone_book) {
        std::cout << pair.first << " - " << pair.second << std::endl;
    }
}

int main() {

    phone_book["tyler"] = "123-456-7890";
    phone_book["alice"] = "111-111-2222";
    phone_book["bob"]   = "1-800-222-3456";

    std::cout << lookup("bob") << std::endl;

    std::cout << "before remove, size: " << phone_book.size() << std::endl;
    remove_contact(std::string("alice"));
    std::cout << "after remove, size: " << phone_book.size() << std::endl;

    print_all_contacts();

    return 0;
}