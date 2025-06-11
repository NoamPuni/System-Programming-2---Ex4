#include <iostream>
#include <string>
#include "MyContainer.hpp" // הכללת קובץ הכותרת של MyContainer
#include <vector>
#include <algorithm>
#include <stdexcept>

int main() {
    
    // יצירת מכולה של מספרים שלמים
    MyContainer<int> intContainer;
    intContainer.addElement(10);
    intContainer.addElement(20);
    intContainer.addElement(30);
    intContainer.addElement(20); // הוספת כפילות

    std::cout << "Initial intContainer: " << intContainer << std::endl;
    std::cout << "Size of intContainer: " << intContainer.size() << std::endl;

    try {
        intContainer.removeElement(20); // הסרת מופע אחד של 20
        std::cout << "intContainer after removing 20: " << intContainer << std::endl;
        std::cout << "Size of intContainer: " << intContainer.size() << std::endl;

        intContainer.removeElement(50); // ניסיון להסיר איבר שלא קיים
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << std::endl;

    // יצירת מכולה של מחרוזות
    MyContainer<std::string> stringContainer;
    stringContainer.addElement("Apple");
    stringContainer.addElement("Banana");
    stringContainer.addElement("Cherry");

    std::cout << "Initial stringContainer: " << stringContainer << std::endl;
    std::cout << "Size of stringContainer: " << stringContainer.size() << std::endl;

    try {
        stringContainer.removeElement("Banana");
        std::cout << "stringContainer after removing Banana: " << stringContainer << std::endl;
        std::cout << "Size of stringContainer: " << stringContainer.size() << std::endl;

        stringContainer.removeElement("Grape");
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}