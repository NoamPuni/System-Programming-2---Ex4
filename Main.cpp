#include <iostream>
#include <string>
#include "MyContainer.hpp" // הכללת קובץ הכותרת של MyContainer
#include <vector>
#include <algorithm>
#include <stdexcept>

using namespace Container;
int main() {
    
    // create a container of integers
    MyContainer<int> intContainer;
    intContainer.addElement(10);
    intContainer.addElement(20);
    intContainer.addElement(30);
    intContainer.addElement(20); // Adding a duplicate

    std::cout << "Initial intContainer: " << intContainer << std::endl;
    std::cout << "Size of intContainer: " << intContainer.size() << std::endl;

    try {
        intContainer.removeElement(20); // Remove one instance of 20
        std::cout << "intContainer after removing 20: " << intContainer << std::endl;
        std::cout << "Size of intContainer: " << intContainer.size() << std::endl;

        intContainer.removeElement(50); // Attempting to remove a non-existent element
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << std::endl;

    // create a container of strings
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

    std::cout << "\n--- Demonstrating Iterators ---" << std::endl;

    // Declare and initialize testContainer here
    MyContainer<int> testContainer; // <--- This line was missing!
    testContainer.addElement(7);
    testContainer.addElement(15);
    testContainer.addElement(6);
    testContainer.addElement(1);
    testContainer.addElement(2);

    std::cout << "Original container for iteration tests: " << testContainer << std::endl;

    // demonstrate OrderIterator
    std::cout << "\nOrder (Insertion) Iteration: ";
    for (auto it = testContainer.begin_order(); it != testContainer.end_order(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl; // Expected: 7 15 6 1 2

    // demonstrate AscendingOrderIterator
    std::cout << "Ascending Order Iteration: ";
    for (auto it = testContainer.begin_ascending_order(); it != testContainer.end_ascending_order(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl; // Expected: 1 2 6 7 15

    // demonstrate DescendingOrderIterator
    std::cout << "Descending Order Iteration: ";
    for (auto it = testContainer.begin_descending_order(); it != testContainer.end_descending_order(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl; // Expected: 15 7 6 2 1

    // demonstrate ReverseOrderIterator
    std::cout << "Reverse Order Iteration: ";
    for (auto it = testContainer.begin_reverse_order(); it != testContainer.end_reverse_order(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl; // Expected: 2 1 6 15 7

    // demonstrate SideCrossOrderIterator
    std::cout << "SideCross Order Iteration: ";
    for (auto it = testContainer.begin_side_cross_order(); it != testContainer.end_side_cross_order(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl; // Expected: 1 15 2 7 6

    // demonstrate MiddleOutOrderIterator
    std::cout << "MiddleOut Order Iteration: ";
    for (auto it = testContainer.begin_middle_out_order(); it != testContainer.end_middle_out_order(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl; // Expected: 6 15 1 7 2

    return 0;
}