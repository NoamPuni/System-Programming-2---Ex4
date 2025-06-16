#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>
#include "MyContainer.hpp"
using namespace Container;
TEST_CASE("MyContainer basic operations") {

    SUBCASE("Adding elements and checking size") {
        MyContainer<int> container;
        CHECK(container.size() == 0); // Check that the container is initially empty

        container.addElement(10);
        CHECK(container.size() == 1);
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 10) != container.getElements().end());

        container.addElement(20);
        container.addElement(30);
        CHECK(container.size() == 3);
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 20) != container.getElements().end());
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 30) != container.getElements().end());
    }

    SUBCASE("Removing existing elements") {
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(20);
        container.addElement(30);
        CHECK(container.size() == 3);

        container.removeElement(20);
        CHECK(container.size() == 2);
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 20) == container.getElements().end());
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 10) != container.getElements().end());
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 30) != container.getElements().end());

        // Test removing an element that has duplicates, verifying all occurrences are removed
        container.addElement(10); // Container: [10, 30, 10]
        container.addElement(40); // Container: [10, 30, 10, 40]
        CHECK(container.size() == 4);

        container.removeElement(10); // Removes all occurrences of 10
        CHECK(container.size() == 2); // Should now contain 30 and 40
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 10) == container.getElements().end());
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 30) != container.getElements().end());
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 40) != container.getElements().end());
    }

    SUBCASE("Removing non-existent elements throws exception") {
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(20);

        CHECK_THROWS_AS(container.removeElement(50), std::runtime_error);
        CHECK(container.size() == 2); // make sure size remains unchanged
    }

    SUBCASE("Removing from an empty container throws exception") {
        MyContainer<int> container;
        CHECK_THROWS_AS(container.removeElement(10), std::runtime_error);
        CHECK(container.size() == 0);
    }

    SUBCASE("Operator<< displays elements correctly") {
        MyContainer<int> container;
        container.addElement(1);
        container.addElement(2);
        container.addElement(3);

        std::stringstream ss;
        ss << container;
        CHECK(ss.str() == "MyContainer elements: [1, 2, 3]");

        MyContainer<std::string> stringContainer;
        stringContainer.addElement("Hello");
        stringContainer.addElement("World");

        std::stringstream ss_str;
        ss_str << stringContainer;
        CHECK(ss_str.str() == "MyContainer elements: [Hello, World]");

        MyContainer<double> doubleContainer;
        std::stringstream ss_empty;
        ss_empty << doubleContainer;
        CHECK(ss_empty.str() == "MyContainer elements: []");
    }

    SUBCASE("Handling duplicate elements correctly in add and remove") {
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(20);
        container.addElement(10); // Add a duplicate
        container.addElement(30);
        CHECK(container.size() == 4); // Container: [10, 20, 10, 30]

        // Verify all occurrences of 10 are removed
        container.removeElement(10);
        CHECK(container.size() == 2); // Should only contain 20 and 30
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 10) == container.getElements().end());
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 20) != container.getElements().end());
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 30) != container.getElements().end());

        // Add more duplicates and test again
        container.addElement(20); // Container: [20, 30, 20]
        container.addElement(20); // Container: [20, 30, 20, 20]
        CHECK(container.size() == 4);

        container.removeElement(20); // Removes all occurrences of 20
        CHECK(container.size() == 1); // Should only contain 30
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 20) == container.getElements().end());
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 30) != container.getElements().end());
    }
}

TEST_CASE("OrderIterator operations") {

    SUBCASE("OrderIterator on non-empty container") {
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(20);
        container.addElement(30);

        // Test begin_order() and iteration
        MyContainer<int>::OrderIterator it = container.begin_order();
        CHECK(*it == 10);
        ++it;
        CHECK(*it == 20);
        it++; // Test post-increment
        CHECK(*it == 30);
        ++it;

        // Test end_order() and comparison
        CHECK(it == container.end_order());
        CHECK_FALSE(it != container.end_order());
    }

    SUBCASE("OrderIterator on empty container") {
        MyContainer<int> container;

        // begin_order() should be equal to end_order() for an empty container
        MyContainer<int>::OrderIterator it_begin = container.begin_order();
        MyContainer<int>::OrderIterator it_end = container.end_order();
        CHECK(it_begin == it_end);
        CHECK_FALSE(it_begin != it_end);

        // Dereferencing an empty iterator should throw
        CHECK_THROWS_AS(*it_begin, std::out_of_range);
    }

    SUBCASE("OrderIterator traversal and all elements access") {
        MyContainer<std::string> container;
        container.addElement("apple");
        container.addElement("banana");
        container.addElement("cherry");
        container.addElement("date");

        std::vector<std::string> expected_order = {"apple", "banana", "cherry", "date"};
        std::vector<std::string> actual_order;

        for (auto iter = container.begin_order(); iter != container.end_order(); ++iter) {
            actual_order.push_back(*iter);
        }

        CHECK(actual_order.size() == expected_order.size());
        if (actual_order.size() == expected_order.size()) {
            for (size_t i = 0; i < actual_order.size(); ++i) {
                CHECK(actual_order[i] == expected_order[i]);
            }
        }
    }

    SUBCASE("OrderIterator dereference out of bounds throws") {
        MyContainer<double> container;
        container.addElement(1.1);

        MyContainer<double>::OrderIterator it = container.begin_order();
        ++it; // Move past the last element
        CHECK_THROWS_AS(*it, std::out_of_range); // Attempt to dereference past-the-end

        // Also check if dereferencing end_order() throws
        MyContainer<double>::OrderIterator it_end = container.end_order();
        CHECK_THROWS_AS(*it_end, std::out_of_range);
    }

    SUBCASE("OrderIterator post-increment behavior") {
        MyContainer<char> container;
        container.addElement('a');
        container.addElement('b');
        container.addElement('c'); // Added 'c' for more comprehensive test to ensure 'it' doesn't go out of bounds too quickly.

        MyContainer<char>::OrderIterator it = container.begin_order();

        // Use copy constructor: creates a new iterator 'prev_it' that is a copy of 'it' BEFORE 'it' is incremented.
        MyContainer<char>::OrderIterator prev_it = it++; 
        CHECK(*prev_it == 'a'); // prev_it should point to 'a' (the value before increment)
        CHECK(*it == 'b');      // 'it' should point to 'b' (the value after increment)

        // To test the next step, create a *new* iterator to capture the state before the next increment.
        // We cannot use assignment (prev_it = ...) for OrderIterator due to its const reference member.
        MyContainer<char>::OrderIterator another_prev_it = it++; // 'it' is 'b', 'another_prev_it' gets 'b', 'it' advances to 'c'
        CHECK(*another_prev_it == 'b'); // 'another_prev_it' should point to 'b'
        CHECK(*it == 'c');             // 'it' should now point to 'c'

        // Advance 'it' to the end.
        MyContainer<char>::OrderIterator last_prev_it = it++; // 'it' is 'c', 'last_prev_it' gets 'c', 'it' advances to end
        CHECK(*last_prev_it == 'c');
        CHECK(it == container.end_order()); // 'it' should now be at the end position
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing an iterator at or past end() should throw
    }

    SUBCASE("OrderIterator multiple dereferences") {
        MyContainer<int> container;
        container.addElement(5);
        
        MyContainer<int>::OrderIterator it = container.begin_order();
        CHECK(*it == 5);
        CHECK(*it == 5); // Dereference multiple times
        CHECK(*it == 5);
        ++it;
        CHECK(it == container.end_order()); // Check if it advanced correctly
    }

    SUBCASE("Iterator comparison with different container instances") {
        MyContainer<int> container1;
        container1.addElement(10);
        container1.addElement(20);

        MyContainer<int> container2;
        container2.addElement(10);
        container2.addElement(20);

        // Iterators from different container instances should be considered unequal,
        // even if they point to logically equivalent elements and indexes.
        // This is because they refer to different MyContainer objects.
        MyContainer<int>::OrderIterator it1_begin = container1.begin_order();
        MyContainer<int>::OrderIterator it2_begin = container2.begin_order();

        CHECK(it1_begin != it2_begin); // Different container instances, so iterators are unequal

        // Advance both iterators
        ++it1_begin;
        ++it2_begin;
        CHECK(it1_begin != it2_begin); // Still unequal as they refer to different containers

        // Iterators reaching their respective ends should also be unequal if from different containers
        MyContainer<int>::OrderIterator it1_end = container1.end_order();
        MyContainer<int>::OrderIterator it2_end = container2.end_order();
        CHECK(it1_end != it2_end); // End iterators from different containers are also unequal
    }

    SUBCASE("OrderIterator behavior after container modification (live iterator verification)") {
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(20);
        container.addElement(30);
        CHECK(container.size() == 3); // Container: [10, 20, 30]

        MyContainer<int>::OrderIterator it = container.begin_order(); // 'it' points to 10 (index 0)

        // Modify the container: remove 20, add 5.
        // As OrderIterator is a live iterator (holds a const reference to the container),
        // it should reflect these changes. The elements vector of the container changes.
        container.removeElement(20); // Container: [10, 30]
        container.addElement(5);    // Container: [10, 30, 5]
        CHECK(container.size() == 3);

        // 'it' was at index 0, which is still 10.
        CHECK(*it == 10);

        // Advance 'it' to the next element. It should now point to the element at index 1 of the *modified* container.
        ++it; // 'it' advances to index 1. In the modified container [10, 30, 5], index 1 holds 30.
        CHECK(*it == 30);

        // Advance 'it' again. It should now point to the element at index 2 of the *modified* container.
        ++it; // 'it' advances to index 2. In the modified container [10, 30, 5], index 2 holds 5.
        CHECK(*it == 5);

        // Advance 'it' to the end.
        ++it;
        CHECK(it == container.end_order()); // 'it' should now be at the end position

        // A newly created iterator should also reflect the current state of the container.
        MyContainer<int>::OrderIterator new_iterator = container.begin_order();
        CHECK(*new_iterator == 10);
        ++new_iterator;
        CHECK(*new_iterator == 30);
        ++new_iterator;
        CHECK(*new_iterator == 5);
        ++new_iterator;
        CHECK(new_iterator == container.end_order());
    }

    SUBCASE("Multiple OrderIterators on the same container") {
        MyContainer<int> container;
        container.addElement(30);
        container.addElement(10);
        container.addElement(20);
        // Container elements in insertion order: [30, 10, 20]

        // Both iterators refer to the same underlying MyContainer instance.
        // They are independent iterators, but will reflect any changes to the container.
        MyContainer<int>::OrderIterator it1 = container.begin_order();
        MyContainer<int>::OrderIterator it2 = container.begin_order(); 

        CHECK(it1 == it2); // Initially, both are at the same position in the same container
        CHECK(*it1 == 30); // Points to the first element (30)
        CHECK(*it2 == 30); // Points to the first element (30)

        ++it1; // it1 is now at index 1 (value 10)
        CHECK(*it1 == 10);
        CHECK(*it2 == 30); // it2 should remain at the first element

        CHECK(it1 != it2); // They are no longer equal

        ++it2; // it2 is now at index 1 (value 10)
        CHECK(it1 == it2); // Now they are equal again
        CHECK(*it1 == 10);
        CHECK(*it2 == 10);

        // Advance both to the end independently
        ++it1; // it1 is now at index 2 (value 20)
        ++it1; // it1 is now at end()
        CHECK(it1 == container.end_order());

        ++it2; // it2 is now at index 2 (value 20)
        ++it2; // it2 is now at end()
        CHECK(it2 == container.end_order());

        CHECK(it1 == it2); // Both are at the end position
    }
}

TEST_CASE("AscendingOrderIterator operations") {

    // Basic Operations and Expected Order
    SUBCASE("Iterating through elements in ascending order for various types") {
        MyContainer<int> intContainer;
        intContainer.addElement(7);
        intContainer.addElement(1);
        intContainer.addElement(15);
        intContainer.addElement(2);
        intContainer.addElement(6);

        std::vector<int> expected_int_order = {1, 2, 6, 7, 15};
        std::vector<int> actual_int_order;

        for (auto it = intContainer.begin_ascending_order(); it != intContainer.end_ascending_order(); ++it) {
            actual_int_order.push_back(*it);
        }
        CHECK(actual_int_order == expected_int_order);

        MyContainer<std::string> stringContainer;
        stringContainer.addElement("cherry");
        stringContainer.addElement("apple");
        stringContainer.addElement("banana");

        std::vector<std::string> expected_string_order = {"apple", "banana", "cherry"};
        std::vector<std::string> actual_string_order;

        for (auto it = stringContainer.begin_ascending_order(); it != stringContainer.end_ascending_order(); ++it) {
            actual_string_order.push_back(*it);
        }
        CHECK(actual_string_order == expected_string_order);
    }

    SUBCASE("Iterator on a single element container") {
        MyContainer<double> container;
        container.addElement(3.14);

        MyContainer<double>::AscendingOrderIterator it = container.begin_ascending_order();
        CHECK(*it == 3.14);
        
        ++it; // Advance to end
        CHECK(it == container.end_ascending_order()); // Iterator should now be at the end
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing end_ascending_order() should throw
    }

    SUBCASE("Basic pre-increment and dereference") {
        MyContainer<int> container;
        container.addElement(30);
        container.addElement(10);
        container.addElement(20);

        MyContainer<int>::AscendingOrderIterator it = container.begin_ascending_order(); // Should point to 10
        CHECK(*it == 10); 
        
        ++it; // it now points to 20
        CHECK(*it == 20);

        ++it; // it now points to 30
        CHECK(*it == 30);

        ++it; // it now points to end_ascending_order()
        CHECK(it == container.end_ascending_order()); 
        CHECK_THROWS_AS(*it, std::out_of_range); // Attempt to dereference end_ascending_order()
    }

    SUBCASE("Basic post-increment behavior") {
        MyContainer<char> container;
        container.addElement('c'); // Original elements order: 'c', 'a'
        container.addElement('a');

        // AscendingOrderIterator will sort elements: 'a', 'c'
        MyContainer<char>::AscendingOrderIterator it = container.begin_ascending_order(); // 'it' initially points to 'a'

        // Use copy constructor: 'prev_it' is initialized with the current state of 'it'
        // before 'it' is incremented.
        MyContainer<char>::AscendingOrderIterator prev_it = it++; // 'prev_it' stores 'a', 'it' advances to 'c'

        CHECK(*prev_it == 'a'); // 'prev_it' should point to 'a'
        CHECK(*it == 'c');      // 'it' should point to 'c'

        // Create a *new* iterator to capture the state before the next increment.
        // Copy assignment (e.g., 'prev_it = it++;') is deleted for AscendingOrderIterator.
        MyContainer<char>::AscendingOrderIterator another_prev_it = it++; // 'another_prev_it' stores 'c', 'it' advances to end
        
        CHECK(*another_prev_it == 'c'); // 'another_prev_it' should point to 'c'
        CHECK(it == container.end_ascending_order()); // 'it' should now be at end_ascending_order()
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing an iterator at or past end() should throw
    }

    // Edge Cases with Empty Container
    SUBCASE("OrderIterator on empty container") {
        MyContainer<int> container;

        // begin_ascending_order() should be equal to end_ascending_order() for an empty container
        MyContainer<int>::AscendingOrderIterator it_begin = container.begin_ascending_order();
        MyContainer<int>::AscendingOrderIterator it_end = container.end_ascending_order();
        CHECK(it_begin == it_end);
        CHECK_FALSE(it_begin != it_end);

        // Dereferencing begin_ascending_order() on an empty container should throw
        CHECK_THROWS_AS(*it_begin, std::out_of_range);
        // Dereferencing end_ascending_order() on an empty container should also throw
        CHECK_THROWS_AS(*it_end, std::out_of_range);

        // Incrementing an iterator on an empty container should still result in end_ascending_order()
        ++it_begin;
        CHECK(it_begin == it_end);
        CHECK_THROWS_AS(*it_begin, std::out_of_range); // Still throws on dereference
    }

    // Edge Cases with Iteration and Dereference
    SUBCASE("Dereferencing an iterator pointing to end_ascending_order() throws") {
        MyContainer<int> container;
        container.addElement(1);
        container.addElement(2);
        
        MyContainer<int>::AscendingOrderIterator it_end = container.end_ascending_order();
        CHECK_THROWS_AS(*it_end, std::out_of_range);

        MyContainer<int>::AscendingOrderIterator it = container.begin_ascending_order();
        ++it; 
        ++it; // Now 'it' is at end_ascending_order()
        CHECK_THROWS_AS(*it, std::out_of_range);
    }

    SUBCASE("Pre-incrementing an iterator past end_ascending_order() does not throw but keeps it at end") {
        MyContainer<int> container;
        container.addElement(100);

        MyContainer<int>::AscendingOrderIterator it = container.begin_ascending_order();
        ++it; // Now 'it' is at end_ascending_order()

        CHECK(it == container.end_ascending_order()); 
        CHECK_THROWS_AS(*it, std::out_of_range); // Still throws on dereference

        ++it; // Incrementing past end_ascending_order()
        CHECK(it == container.end_ascending_order()); // Should still be equal to end_ascending_order()
        CHECK_THROWS_AS(*it, std::out_of_range); // Should still throw on dereference
    }

    SUBCASE("Post-incrementing an iterator past end_ascending_order() does not throw but keeps it at end") {
        MyContainer<int> container;
        container.addElement(100);

        MyContainer<int>::AscendingOrderIterator it = container.begin_ascending_order();
        MyContainer<int>::AscendingOrderIterator old_it = it++; // old_it is begin, it is end

        CHECK(*old_it == 100);
        CHECK(it == container.end_ascending_order());
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing the advanced 'it' throws

        MyContainer<int>::AscendingOrderIterator old_it_2 = it++; // old_it_2 is end, it is still end
        CHECK(old_it_2 == container.end_ascending_order());
        CHECK(it == container.end_ascending_order());
        CHECK_THROWS_AS(*old_it_2, std::out_of_range); // Dereferencing old_it_2 throws
        CHECK_THROWS_AS(*it, std::out_of_range);      // Dereferencing it throws
    }

    // Iterator Comparison Logic
   SUBCASE("AscendingOrderIterator comparison with different container instances") {
        MyContainer<int> container1;
        container1.addElement(30);
        container1.addElement(10);
        container1.addElement(20); // Sorted order for container1: [10, 20, 30]

        MyContainer<int> container2;
        container2.addElement(10);
        container2.addElement(30);
        container2.addElement(20); // Sorted order for container2: [10, 20, 30]

        // Iterators from different container instances should be considered unequal,
        // even if they point to logically equivalent elements and indexes in their respective snapshots.
        // This is because they refer to different MyContainer objects.
        MyContainer<int>::AscendingOrderIterator it1_begin = container1.begin_ascending_order();
        MyContainer<int>::AscendingOrderIterator it2_begin = container2.begin_ascending_order();

        CHECK(it1_begin != it2_begin); // Different container instances, so iterators are unequal

        // Advance both iterators
        ++it1_begin;
        ++it2_begin;
        CHECK(it1_begin != it2_begin); // Still unequal as they refer to different containers

        // Iterators reaching their respective ends should also be unequal if from different containers
        MyContainer<int>::AscendingOrderIterator it1_end = container1.end_ascending_order();
        MyContainer<int>::AscendingOrderIterator it2_end = container2.end_ascending_order();
        CHECK(it1_end != it2_end); // End iterators from different containers are also unequal
    }

    SUBCASE("Iterator comparison within the same container at different positions") {
        MyContainer<int> container;
        container.addElement(1);
        container.addElement(2);
        container.addElement(3);

        MyContainer<int>::AscendingOrderIterator it1 = container.begin_ascending_order(); // points to 1
        MyContainer<int>::AscendingOrderIterator it2 = container.begin_ascending_order(); // points to 1
        MyContainer<int>::AscendingOrderIterator it3 = container.begin_ascending_order();
        ++it3; // points to 2

        CHECK(it1 == it2); // Same position, same container -> equal
        CHECK_FALSE(it1 != it2);

        CHECK(it1 != it3); // Different positions, same container -> unequal
        CHECK_FALSE(it1 == it3);

        CHECK(it2 != it3);
        CHECK_FALSE(it2 == it3);
    }

    // Iterator Behavior with Container modifications 
     SUBCASE("AscendingOrderIterator behavior after container modification (index snapshot)") {
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(30);
        container.addElement(20);
        CHECK(container.size() == 3); // Original elements: [10, 30, 20]

        // Iterator captures initial state: sorted values [10, 20, 30]
        MyContainer<int>::AscendingOrderIterator it_asc_initial = container.begin_ascending_order(); 

        // Verify initial iterator
        CHECK(*it_asc_initial == 10);
        ++it_asc_initial;
        CHECK(*it_asc_initial == 20);
        ++it_asc_initial;
        CHECK(*it_asc_initial == 30);
        ++it_asc_initial;
        CHECK(it_asc_initial == container.end_ascending_order());


        // --- Modify the container ---
        // New container elements: [10, 30, 5] (after removal and addition)
        container.removeElement(20); 
        container.addElement(5);     
        CHECK(container.size() == 3); 

        // New iterator created after modification, captures the *current* state.
        // Current container sorted: [5, 10, 30]
        MyContainer<int>::AscendingOrderIterator it_asc_after_mod = container.begin_ascending_order(); 

        // Verify new iterator reflects the modified container's sorted order.
        CHECK(*it_asc_after_mod == 5);  
        ++it_asc_after_mod; 
        CHECK(*it_asc_after_mod == 10); 
        ++it_asc_after_mod;
        CHECK(*it_asc_after_mod == 30); 
        ++it_asc_after_mod;
        CHECK(it_asc_after_mod == container.end_ascending_order());
    }

   SUBCASE("Multiple AscendingOrderIterators on the same container (independent snapshots)") {
        MyContainer<int> container;
        container.addElement(30);
        container.addElement(10);
        container.addElement(20);
        // Container elements initially: [30, 10, 20]

        // AscendingOrderIterator creates an independent snapshot of sorted indexes
        // at the time of its construction.
        // Sorted order based on initial elements: [10, 20, 30]
        MyContainer<int>::AscendingOrderIterator it1 = container.begin_ascending_order(); 
        MyContainer<int>::AscendingOrderIterator it2 = container.begin_ascending_order(); 

        // Although they are independent objects, if created at the same logical position
        // from the same container, their equality operator might return true based on index and container reference.
        CHECK(it1 == it2); // Initially, both are at the same logical position (start)

        CHECK(*it1 == 10); // Both point to the first element in their respective sorted snapshots
        CHECK(*it2 == 10); 

        ++it1; // it1 advances to the next element in its snapshot (20)
        CHECK(*it1 == 20);
        CHECK(*it2 == 10); // it2 should remain at 10

        CHECK(it1 != it2); // They are no longer at the same logical position

        ++it2; // it2 advances to the next element in its snapshot (20)
        CHECK(it1 == it2); // Now they are at the same logical position again
        CHECK(*it1 == 20);
        CHECK(*it2 == 20);

        // Advance both to the end independently
        ++it1; // it1 is now at 30
        ++it1; // it1 is now at end()
        CHECK(it1 == container.end_ascending_order()); // Check against an end iterator of the same container

        ++it2; // it2 is now at 30
        ++it2; // it2 is now at end()
        CHECK(it2 == container.end_ascending_order()); // Check against an end iterator of the same container

        CHECK(it1 == it2); // Both are at their respective end positions
    }
}

TEST_CASE("DescendingOrderIterator operations") {

    // Basic Operations and Expected Order
    SUBCASE("Iterating through elements in descending order for various types") {
        MyContainer<int> intContainer;
        intContainer.addElement(7);
        intContainer.addElement(1);
        intContainer.addElement(15);
        intContainer.addElement(2);
        intContainer.addElement(6);

        std::vector<int> expected_int_order = {15, 7, 6, 2, 1}; 
        std::vector<int> actual_int_order;

        for (auto it = intContainer.begin_descending_order(); it != intContainer.end_descending_order(); ++it) { 
            actual_int_order.push_back(*it);
        }
        CHECK(actual_int_order == expected_int_order);

        MyContainer<std::string> stringContainer;
        stringContainer.addElement("cherry");
        stringContainer.addElement("apple");
        stringContainer.addElement("banana");

        std::vector<std::string> expected_string_order = {"cherry", "banana", "apple"}; 
        std::vector<std::string> actual_string_order;

        for (auto it = stringContainer.begin_descending_order(); it != stringContainer.end_descending_order(); ++it) { 
            actual_string_order.push_back(*it);
        }
        CHECK(actual_string_order == expected_string_order);
    }

    SUBCASE("Iterator on a single element container (DescendingOrder)") { 
        MyContainer<double> container;
        container.addElement(3.14);

        MyContainer<double>::DescendingOrderIterator it = container.begin_descending_order(); 
        CHECK(*it == 3.14);
        
        ++it; // Advance to end
        CHECK(it == container.end_descending_order()); 
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing end_descending_order() should throw
    }

    SUBCASE("Basic pre-increment and dereference (DescendingOrder)") { 
        MyContainer<int> container;
        container.addElement(30);
        container.addElement(10);
        container.addElement(20);

        MyContainer<int>::DescendingOrderIterator it = container.begin_descending_order(); // Should point to 30
        CHECK(*it == 30); 
        
        ++it; // it now points to 20
        CHECK(*it == 20); // Value remains 20

        ++it; // it now points to 10
        CHECK(*it == 10); 

        ++it; // it now points to end_descending_order()
        CHECK(it == container.end_descending_order()); 
        CHECK_THROWS_AS(*it, std::out_of_range); // Attempt to dereference end_descending_order() 
    }

    SUBCASE("Basic post-increment behavior (DescendingOrder)") { 
        MyContainer<char> container;
        container.addElement('c'); // Original elements order: 'c', 'a'
        container.addElement('a');

        // DescendingOrderIterator will sort elements: 'c', 'a'
        MyContainer<char>::DescendingOrderIterator it = container.begin_descending_order(); // 'it' initially points to 'c'

        // Use copy constructor: 'prev_it' is initialized with the current state of 'it'
        // before 'it' is incremented.
        MyContainer<char>::DescendingOrderIterator prev_it = it++; // 'prev_it' stores 'c', 'it' advances to 'a'

        CHECK(*prev_it == 'c'); // 'prev_it' should point to 'c'
        CHECK(*it == 'a');      // 'it' should point to 'a'

        // Create a *new* iterator to capture the state before the next increment.
        // Copy assignment (e.g., 'prev_it = it++;') is deleted for DescendingOrderIterator.
        MyContainer<char>::DescendingOrderIterator another_prev_it = it++; // 'another_prev_it' stores 'a', 'it' advances to end

        CHECK(*another_prev_it == 'a'); // 'another_prev_it' should point to 'a'
        CHECK(it == container.end_descending_order()); // 'it' should now be at end_descending_order()
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing an iterator at or past end() should throw
    }

    // Edge Cases with Empty Container
    SUBCASE("DescendingOrderIterator on empty container") { 
        MyContainer<int> container;

        // begin_descending_order() should be equal to end_descending_order() for an empty container
        MyContainer<int>::DescendingOrderIterator it_begin = container.begin_descending_order();
        MyContainer<int>::DescendingOrderIterator it_end = container.end_descending_order();
        CHECK(it_begin == it_end);
        CHECK_FALSE(it_begin != it_end);

        // Dereferencing begin_descending_order() on an empty container should throw    
        CHECK_THROWS_AS(*it_begin, std::out_of_range);
        // Dereferencing end_descending_order() on an empty container should also throw
        CHECK_THROWS_AS(*it_end, std::out_of_range);

        // Incrementing an iterator on an empty container should still result in end_descending_order()
        ++it_begin;
        CHECK(it_begin == it_end);
        CHECK_THROWS_AS(*it_begin, std::out_of_range); // Still throws on dereference
    }

    // Edge Cases with Iteration and Dereference
    SUBCASE("Dereferencing an iterator pointing to end_descending_order() throws") { 
        MyContainer<int> container;
        container.addElement(1);
        container.addElement(2);
        
        MyContainer<int>::DescendingOrderIterator it_end = container.end_descending_order(); 
        CHECK_THROWS_AS(*it_end, std::out_of_range);

        MyContainer<int>::DescendingOrderIterator it = container.begin_descending_order();
        ++it;
        ++it; // Now 'it' is at end_descending_order()
        CHECK_THROWS_AS(*it, std::out_of_range);
    }

    SUBCASE("Pre-incrementing an iterator past end_descending_order() does not throw but keeps it at end") { 
        MyContainer<int> container;
        container.addElement(100);

        MyContainer<int>::DescendingOrderIterator it = container.begin_descending_order();
        ++it; // Now 'it' is at end_descending_order()

        CHECK(it == container.end_descending_order());
        CHECK_THROWS_AS(*it, std::out_of_range); // Still throws on dereference

        ++it; // Incrementing past end_descending_order()
        CHECK(it == container.end_descending_order()); // Should still be equal to end_descending_order()  
        CHECK_THROWS_AS(*it, std::out_of_range); // Should still throw on dereference
    }

    SUBCASE("Post-incrementing an iterator past end_descending_order() does not throw but keeps it at end") { 
        MyContainer<int> container;
        container.addElement(100);

        MyContainer<int>::DescendingOrderIterator it = container.begin_descending_order();
        MyContainer<int>::DescendingOrderIterator old_it = it++;

        CHECK(*old_it == 100);
        CHECK(it == container.end_descending_order()); 
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing the advanced 'it' throws

        MyContainer<int>::DescendingOrderIterator old_it_2 = it++; // old_it_2 is end, it is still end
        CHECK(old_it_2 == container.end_descending_order()); 
        CHECK(it == container.end_descending_order()); 
        CHECK_THROWS_AS(*old_it_2, std::out_of_range); // Dereferencing old_it_2 throws
        CHECK_THROWS_AS(*it, std::out_of_range);       // Dereferencing it throws
    }

    // Iterator Comparison Logic
    SUBCASE("DescendingOrderIterator comparison with different container instances") { 
        MyContainer<int> container1;
        container1.addElement(30);
        container1.addElement(10);
        container1.addElement(20); // Sorted order for container1: [30, 20, 10] 

        MyContainer<int> container2;
        container2.addElement(10);
        container2.addElement(30);
        container2.addElement(20); // Sorted order for container2: [30, 20, 10] 

        // Iterators from different container instances should be considered unequal,
        // even if they point to logically equivalent elements and indexes in their respective snapshots.
        // This is because they refer to different MyContainer objects.
        MyContainer<int>::DescendingOrderIterator it1_begin = container1.begin_descending_order();
        MyContainer<int>::DescendingOrderIterator it2_begin = container2.begin_descending_order();

        CHECK(it1_begin != it2_begin); // Different container instances, so iterators are unequal

        // Advance both iterators
        ++it1_begin;
        ++it2_begin;
        CHECK(it1_begin != it2_begin); // Still unequal as they refer to different containers

        // Iterators reaching their respective ends should also be unequal if from different containers
        MyContainer<int>::DescendingOrderIterator it1_end = container1.end_descending_order();
        MyContainer<int>::DescendingOrderIterator it2_end = container2.end_descending_order();
        CHECK(it1_end != it2_end); // End iterators from different containers are also unequal
    }

    SUBCASE("Iterator comparison within the same container at different positions (DescendingOrder)") { 
        MyContainer<int> container;
        container.addElement(1);
        container.addElement(2);
        container.addElement(3);

        MyContainer<int>::DescendingOrderIterator it1 = container.begin_descending_order();  // points to 3
        MyContainer<int>::DescendingOrderIterator it2 = container.begin_descending_order();  // points to 3
        MyContainer<int>::DescendingOrderIterator it3 = container.begin_descending_order();  // points to 3
        ++it3; // points to 2

        CHECK(it1 == it2); // Same position, same container -> equal
        CHECK_FALSE(it1 != it2);

        CHECK(it1 != it3); // Different positions, same container -> unequal
        CHECK_FALSE(it1 == it3);

        CHECK(it2 != it3);
        CHECK_FALSE(it2 == it3);
    }

    // Iterator Behavior with Container modifications 
    SUBCASE("DescendingOrderIterator behavior after container modification (index snapshot)") { 
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(30);
        container.addElement(20);
        CHECK(container.size() == 3); // Original elements: [10, 30, 20]

        // Iterator captures initial state: sorted values [30, 20, 10]
        MyContainer<int>::DescendingOrderIterator it_des_initial = container.begin_descending_order();

        // Verify initial iterator
        CHECK(*it_des_initial == 30);
        ++it_des_initial;
        CHECK(*it_des_initial == 20); // Value remains 20
        ++it_des_initial;
        CHECK(*it_des_initial == 10); 
        ++it_des_initial;
        CHECK(it_des_initial == container.end_descending_order()); 


        // --- Modify the container ---
        // New container elements: [10, 30, 5] (after removal and addition)
        container.removeElement(20); 
        container.addElement(5);     
        CHECK(container.size() == 3); 

        // New iterator created after modification, captures the *current* state.
        // Current container sorted: [30, 10, 5] 
        MyContainer<int>::DescendingOrderIterator it_des_after_mod = container.begin_descending_order(); 

        // Verify new iterator reflects the modified container's sorted order.
        CHECK(*it_des_after_mod == 30);
        ++it_des_after_mod;
        CHECK(*it_des_after_mod == 10);
        ++it_des_after_mod;
        CHECK(*it_des_after_mod == 5);
        ++it_des_after_mod;
        CHECK(it_des_after_mod == container.end_descending_order());
    }

    SUBCASE("Multiple DescendingOrderIterators on the same container (independent snapshots)") { 
        MyContainer<int> container;
        container.addElement(30);
        container.addElement(10);
        container.addElement(20);
        // Container elements initially: [30, 10, 20]

        // DescendingOrderIterator creates an independent snapshot of sorted indexes 
        // at the time of its construction.
        // Sorted order based on initial elements: [30, 20, 10] 
        MyContainer<int>::DescendingOrderIterator it1 = container.begin_descending_order(); 
        MyContainer<int>::DescendingOrderIterator it2 = container.begin_descending_order(); 

        // Although they are independent objects, if created at the same logical position
        // from the same container, their equality operator might return true based on index and container reference.
        CHECK(it1 == it2); // Initially, both are at the same logical position (start)

        CHECK(*it1 == 30); // Both point to the first element in their respective sorted snapshots // Changed expected value
        CHECK(*it2 == 30); // Changed expected value

        ++it1; // it1 advances to the next element in its snapshot (20)
        CHECK(*it1 == 20); // Value remains 20
        CHECK(*it2 == 30); // it2 should remain at 30 // Changed expected value (original was 10, this is now correct for descending)

        CHECK(it1 != it2); // They are no longer at the same logical position

        ++it2; // it2 advances to the next element in its snapshot (20)
        CHECK(it1 == it2); // Now they are at the same logical position again
        CHECK(*it1 == 20); // Value remains 20
        CHECK(*it2 == 20); // Value remains 20

        // Advance both to the end independently
        ++it1; // it1 is now at 10
        ++it1; // it1 is now at end()
        CHECK(it1 == container.end_descending_order()); // Check against an end iterator of the same container 

        ++it2; // it2 is now at 10
        ++it2; // it2 is now at end()
        CHECK(it2 == container.end_descending_order()); // Check against an end iterator of the same container 

        CHECK(it1 == it2); // Both are at their respective end positions
    }
}

TEST_CASE("ReverseOrderIterator operations") {
    SUBCASE("ReverseOrderIterator on non-empty container") {
        MyContainer<int> container;
        container.addElement(10); // Index 0
        container.addElement(20); // Index 1
        container.addElement(30); // Index 2

        // Test begin_reverse_order() and iteration
        MyContainer<int>::ReverseOrderIterator it = container.begin_reverse_order();
        CHECK(*it == 30); // Should be the last element (index 2)
        ++it;
        CHECK(*it == 20); // Should be the second to last element (index 1)
        it++; // Test post-increment
        CHECK(*it == 10); // Should be the first element (index 0)
        ++it;

        // Test end_reverse_order() and comparison
        CHECK(it == container.end_reverse_order());
        CHECK_FALSE(it != container.end_reverse_order());
    }

    SUBCASE("ReverseOrderIterator on empty container") {
        MyContainer<int> container;

        // begin_reverse_order() should be equal to end_reverse_order() for an empty container
        MyContainer<int>::ReverseOrderIterator it_begin = container.begin_reverse_order();
        MyContainer<int>::ReverseOrderIterator it_end = container.end_reverse_order();
        CHECK(it_begin == it_end);
        CHECK_FALSE(it_begin != it_end);

        // Dereferencing an empty iterator should throw
        // This is important because the loop condition `it != container.end_reverse_order()` prevents dereferencing `end_reverse_order()`
        // but if someone manually tries to dereference begin_reverse_order() on an empty container.
        CHECK_THROWS_AS(*it_begin, std::out_of_range);
    }

    SUBCASE("ReverseOrderIterator traversal and all elements access") {
        MyContainer<std::string> container;
        container.addElement("apple");  // Last in reverse
        container.addElement("banana"); // Second to last in reverse
        container.addElement("cherry"); // Third to last in reverse
        container.addElement("date");   // First in reverse

        std::vector<std::string> expected_order = {"date", "cherry", "banana", "apple"}; // Expected reverse order
        std::vector<std::string> actual_order;

        for (auto iter = container.begin_reverse_order(); iter != container.end_reverse_order(); ++iter) {
            actual_order.push_back(*iter);
        }

        CHECK(actual_order.size() == expected_order.size());
        if (actual_order.size() == expected_order.size()) {
            for (size_t i = 0; i < actual_order.size(); ++i) {
                CHECK(actual_order[i] == expected_order[i]);
            }
        }
    }

    SUBCASE("ReverseOrderIterator dereference out of bounds throws") {
        MyContainer<double> container;
        container.addElement(1.1); // Index 0

        MyContainer<double>::ReverseOrderIterator it = container.begin_reverse_order(); // Points to 1.1 (index 0)
        ++it; // Move past the first element (becomes end_reverse_order() state)
        CHECK_THROWS_AS(*it, std::out_of_range); // Attempt to dereference past-the-end

        // Also check if dereferencing end_reverse_order() throws
        MyContainer<double>::ReverseOrderIterator it_end = container.end_reverse_order();
        CHECK_THROWS_AS(*it_end, std::out_of_range);
    }

    SUBCASE("ReverseOrderIterator post-increment behavior") {
        MyContainer<char> container;
        container.addElement('a'); // Index 0
        container.addElement('b'); // Index 1
        container.addElement('c'); // Index 2 (First in reverse order)

        MyContainer<char>::ReverseOrderIterator it = container.begin_reverse_order(); // Points to 'c'

        // Use copy constructor: creates a new iterator 'prev_it' that is a copy of 'it' BEFORE 'it' is incremented.
        MyContainer<char>::ReverseOrderIterator prev_it = it++; 
        CHECK(*prev_it == 'c'); // prev_it should point to 'c' (the value before increment)
        CHECK(*it == 'b');      // 'it' should point to 'b' (the value after increment)

        // To test the next step, creates a *new* iterator to capture the state before the next increment.
        MyContainer<char>::ReverseOrderIterator another_prev_it = it++; // 'it' is 'b', 'another_prev_it' gets 'b', 'it' advances to 'a'
        CHECK(*another_prev_it == 'b'); // 'another_prev_it' should point to 'b'
        CHECK(*it == 'a');              // 'it' should now point to 'a'

        // 'it' advances to the end.
        MyContainer<char>::ReverseOrderIterator last_prev_it = it++; // 'it' is 'a', 'last_prev_it' gets 'a', 'it' advances to end
        CHECK(*last_prev_it == 'a');
        CHECK(it == container.end_reverse_order()); // 'it' should now be at the end position
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing an iterator at or past end() should throw
    }

    SUBCASE("ReverseOrderIterator multiple dereferences") {
        MyContainer<int> container;
        container.addElement(5); // Index 0 (First in reverse order)
        
        MyContainer<int>::ReverseOrderIterator it = container.begin_reverse_order(); // Points to 5
        CHECK(*it == 5);
        CHECK(*it == 5); // Dereference multiple times
        CHECK(*it == 5);
        ++it;
        CHECK(it == container.end_reverse_order()); // Check if it advanced correctly
    }

    SUBCASE("ReverseOrderIterator comparison with different container instances") {
        MyContainer<int> container1;
        container1.addElement(10); // Index 0
        container1.addElement(20); // Index 1 (First in reverse order)

        MyContainer<int> container2;
        container2.addElement(10);
        container2.addElement(20);

        // Iterators from different container instances should be considered unequal,
        // even if they point to logically equivalent elements and indexes.
        // This is because they refer to different MyContainer objects.
        MyContainer<int>::ReverseOrderIterator it1_begin = container1.begin_reverse_order();
        MyContainer<int>::ReverseOrderIterator it2_begin = container2.begin_reverse_order();

        CHECK(it1_begin != it2_begin); // Different container instances, so iterators are unequal

        // Advance both iterators
        ++it1_begin;
        ++it2_begin;
        CHECK(it1_begin != it2_begin); // Still unequal as they refer to different containers

        // Iterators reaching their respective ends should also be unequal if from different containers
        MyContainer<int>::ReverseOrderIterator it1_end = container1.end_reverse_order();
        MyContainer<int>::ReverseOrderIterator it2_end = container2.end_reverse_order();
        CHECK(it1_end != it2_end); // End iterators from different containers are also unequal
    }

    SUBCASE("ReverseOrderIterator behavior after container modification (live iterator verification)") {
        MyContainer<int> container;
        container.addElement(10); // Index 0
        container.addElement(20); // Index 1
        container.addElement(30); // Index 2 (Initial begin_reverse_order points here)
        CHECK(container.size() == 3); // Container: [10, 20, 30]

        MyContainer<int>::ReverseOrderIterator it = container.begin_reverse_order(); // 'it' points to 30 (index 2)

        // Modify the container: remove 20, add 5.
        // As ReverseOrderIterator is a live iterator (holds a const reference to the container),
        // it should reflect these changes. The elements vector of the container changes.
        container.removeElement(20); // Container: [10, 30] (indexes 0, 1)
        container.addElement(5);     // Container: [10, 30, 5] (indexes 0, 1, 2)
        CHECK(container.size() == 3);

        // 'it' was originally pointing to index 2 (which held 30).
        // After modifications, the element at index 2 is now 5.
        CHECK(*it == 5); // 'it' should now point to 5.

        // Advance 'it' to the next element. It should now point to the element at index 1 of the *modified* container.
        ++it; // 'it' advances to index 1. In the modified container [10, 30, 5], index 1 holds 30.
        CHECK(*it == 30);

        // Advance 'it' again. It should now point to the element at index 0 of the *modified* container.
        ++it; // 'it' advances to index 0. In the modified container [10, 30, 5], index 0 holds 10.
        CHECK(*it == 10);

        // Advance 'it' to the end.
        ++it;
        CHECK(it == container.end_reverse_order()); // 'it' should now be at the end position

        // A newly created iterator should also reflect the current state of the container.
        MyContainer<int>::ReverseOrderIterator new_iterator = container.begin_reverse_order(); // Points to 5
        CHECK(*new_iterator == 5);
        ++new_iterator;
        CHECK(*new_iterator == 30);
        ++new_iterator;
        CHECK(*new_iterator == 10);
        ++new_iterator;
        CHECK(new_iterator == container.end_reverse_order());
    }

    SUBCASE("Multiple ReverseOrderIterators on the same container") {
        MyContainer<int> container;
        container.addElement(30); // Index 0 (Last in reverse)
        container.addElement(10); // Index 1 (Middle in reverse)
        container.addElement(20); // Index 2 (First in reverse)
        // Container elements in insertion order: [30, 10, 20]
        // Reverse order: [20, 10, 30]

        // Both iterators refer to the same underlying MyContainer instance.
        // They are independent iterators, but will reflect any changes to the container.
        MyContainer<int>::ReverseOrderIterator it1 = container.begin_reverse_order(); // Points to 20
        MyContainer<int>::ReverseOrderIterator it2 = container.begin_reverse_order(); // Points to 20

        CHECK(it1 == it2); // Initially, both are at the same position in the same container
        CHECK(*it1 == 20); // Points to the last element (20)
        CHECK(*it2 == 20); // Points to the last element (20)

        ++it1; // it1 is now at index 1 (value 10)
        CHECK(*it1 == 10);
        CHECK(*it2 == 20); // it2 should remain at the first element

        CHECK(it1 != it2); // They are no longer equal

        ++it2; // it2 is now at index 1 (value 10)
        CHECK(it1 == it2); // Now they are equal again
        CHECK(*it1 == 10);
        CHECK(*it2 == 10);

        // Advance both to the end independently
        ++it1; // it1 is now at index 0 (value 30)
        ++it1; // it1 is now at end()
        CHECK(it1 == container.end_reverse_order());

        ++it2; // it2 is now at index 0 (value 30)
        ++it2; // it2 is now at end()
        CHECK(it2 == container.end_reverse_order());

        CHECK(it1 == it2); // Both are at the end position
    }
}

TEST_CASE("SideCrossOrderIterator operations") {

    // Basic Operations and Expected Order
    SUBCASE("Iterating through elements in side-cross order for various types") {
        MyContainer<int> intContainer;
        intContainer.addElement(7);
        intContainer.addElement(1);
        intContainer.addElement(15);
        intContainer.addElement(2);
        intContainer.addElement(6);

        // Original: [7, 1, 15, 2, 6]
        // Sorted: [1, 2, 6, 7, 15]
        // SideCross Order: Smallest, Largest, 2nd Smallest, 2nd Largest, Middle
        // Expected: 1, 15, 2, 7, 6
        std::vector<int> expected_int_order = {1, 15, 2, 7, 6};
        std::vector<int> actual_int_order;

        for (auto it = intContainer.begin_side_cross_order(); it != intContainer.end_side_cross_order(); ++it) {
            actual_int_order.push_back(*it);
        }
        CHECK(actual_int_order == expected_int_order);

        MyContainer<std::string> stringContainer;
        stringContainer.addElement("cherry");
        stringContainer.addElement("apple");
        stringContainer.addElement("banana");

        // Original: ["cherry", "apple", "banana"]
        // Sorted: ["apple", "banana", "cherry"]
        // SideCross Order: Smallest, Largest, Middle
        // Expected: "apple", "cherry", "banana"
        std::vector<std::string> expected_string_order = {"apple", "cherry", "banana"};
        std::vector<std::string> actual_string_order;

        for (auto it = stringContainer.begin_side_cross_order(); it != stringContainer.end_side_cross_order(); ++it) {
            actual_string_order.push_back(*it);
        }
        CHECK(actual_string_order == expected_string_order);
    }

    SUBCASE("SideCrossOrderIterator on a single element container") {
        MyContainer<double> container;
        container.addElement(3.14);

        MyContainer<double>::SideCrossOrderIterator it = container.begin_side_cross_order();
        CHECK(*it == 3.14);
        
        ++it; // Advance to end
        CHECK(it == container.end_side_cross_order()); // Iterator should now be at the end
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing end_side_cross_order() should throw
    }

    SUBCASE("Basic pre-increment and dereference for SideCrossOrderIterator") {
        MyContainer<int> container;
        container.addElement(30);
        container.addElement(10);
        container.addElement(20);

        // Original: [30, 10, 20]
        // Sorted: [10, 20, 30]
        // SideCross: 10, 30, 20
        MyContainer<int>::SideCrossOrderIterator it = container.begin_side_cross_order(); // Should point to 10
        CHECK(*it == 10); 
        
        ++it; // it now points to 30
        CHECK(*it == 30);

        ++it; // it now points to 20
        CHECK(*it == 20);

        ++it; // it now points to end_side_cross_order()
        CHECK(it == container.end_side_cross_order()); 
        CHECK_THROWS_AS(*it, std::out_of_range); // Attempt to dereference end_side_cross_order()
    }

    SUBCASE("Basic post-increment behavior for SideCrossOrderIterator") {
        MyContainer<char> container;
        container.addElement('c'); 
        container.addElement('a');
        container.addElement('b');

        // Original: ['c', 'a', 'b']
        // Sorted: ['a', 'b', 'c']
        // SideCross: 'a', 'c', 'b'
        MyContainer<char>::SideCrossOrderIterator it = container.begin_side_cross_order(); // 'it' initially points to 'a'

        MyContainer<char>::SideCrossOrderIterator prev_it = it++; // 'prev_it' stores 'a', 'it' advances to 'c'

        CHECK(*prev_it == 'a'); // 'prev_it' should point to 'a'
        CHECK(*it == 'c'); // 'it' should point to 'c'

        MyContainer<char>::SideCrossOrderIterator another_prev_it = it++; // 'another_prev_it' stores 'c', 'it' advances to 'b'
        
        CHECK(*another_prev_it == 'c'); // 'another_prev_it' should point to 'c'
        CHECK(*it == 'b'); // 'it' should point to 'b'

        MyContainer<char>::SideCrossOrderIterator last_prev_it = it++; // 'last_prev_it' stores 'b', 'it' advances to end
        
        CHECK(*last_prev_it == 'b'); // 'last_prev_it' should point to 'b'
        CHECK(it == container.end_side_cross_order()); // 'it' should now be at end_side_cross_order()
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing an iterator at or past end() should throw
    }

    // Edge Cases with Empty Container
    SUBCASE("SideCrossOrderIterator on empty container") {
        MyContainer<int> container;

        // begin_side_cross_order() should be equal to end_side_cross_order() for an empty container
        MyContainer<int>::SideCrossOrderIterator it_begin = container.begin_side_cross_order();
        MyContainer<int>::SideCrossOrderIterator it_end = container.end_side_cross_order();
        CHECK(it_begin == it_end);
        CHECK_FALSE(it_begin != it_end);

        // Dereferencing begin_side_cross_order() on an empty container should throw
        CHECK_THROWS_AS(*it_begin, std::out_of_range);
        // Dereferencing end_side_cross_order() on an empty container should also throw
        CHECK_THROWS_AS(*it_end, std::out_of_range);

        // Incrementing an iterator on an empty container should still result in end_side_cross_order()
        ++it_begin;
        CHECK(it_begin == it_end);
        CHECK_THROWS_AS(*it_begin, std::out_of_range); // Still throws on dereference
    }

    // Edge Cases with Iteration and Dereference
    SUBCASE("Dereferencing SideCrossOrderIterator pointing to end_side_cross_order() throws") {
        MyContainer<int> container;
        container.addElement(1);
        container.addElement(2);
        
        MyContainer<int>::SideCrossOrderIterator it_end = container.end_side_cross_order();
        CHECK_THROWS_AS(*it_end, std::out_of_range);

        MyContainer<int>::SideCrossOrderIterator it = container.begin_side_cross_order(); // points to 1
        ++it; // points to 2
        ++it; // Now 'it' is at end_side_cross_order()
        CHECK_THROWS_AS(*it, std::out_of_range);
    }

    SUBCASE("Pre-incrementing SideCrossOrderIterator past end_side_cross_order() does not throw but keeps it at end") {
        MyContainer<int> container;
        container.addElement(100);

        MyContainer<int>::SideCrossOrderIterator it = container.begin_side_cross_order();
        ++it; // Now 'it' is at end_side_cross_order()

        CHECK(it == container.end_side_cross_order()); 
        CHECK_THROWS_AS(*it, std::out_of_range); // Still throws on dereference

        ++it; // Incrementing past end_side_cross_order()
        CHECK(it == container.end_side_cross_order()); // Should still be equal to end_side_cross_order()
        CHECK_THROWS_AS(*it, std::out_of_range); // Should still throw on dereference
    }

    SUBCASE("Post-incrementing SideCrossOrderIterator past end_side_cross_order() does not throw but keeps it at end") {
        MyContainer<int> container;
        container.addElement(100);

        MyContainer<int>::SideCrossOrderIterator it = container.begin_side_cross_order();
        MyContainer<int>::SideCrossOrderIterator old_it = it++; // old_it is begin, it is end

        CHECK(*old_it == 100);
        CHECK(it == container.end_side_cross_order());
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing the advanced 'it' throws

        MyContainer<int>::SideCrossOrderIterator old_it_2 = it++; // old_it_2 is end, it is still end
        CHECK(old_it_2 == container.end_side_cross_order());
        CHECK(it == container.end_side_cross_order());
        CHECK_THROWS_AS(*old_it_2, std::out_of_range); // Dereferencing old_it_2 throws
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing it throws
    }

    // Iterator Comparison Logic
    SUBCASE("SideCrossOrderIterator comparison with different container instances") {
        MyContainer<int> container1;
        container1.addElement(30);
        container1.addElement(10);
        container1.addElement(20); // Sorted order for container1: [10, 20, 30]

        MyContainer<int> container2;
        container2.addElement(10);
        container2.addElement(30);
        container2.addElement(20); // Sorted order for container2: [10, 20, 30]

        // Iterators from different container instances should be considered unequal,
        // as they refer to different MyContainer objects.
        MyContainer<int>::SideCrossOrderIterator it1_begin = container1.begin_side_cross_order();
        MyContainer<int>::SideCrossOrderIterator it2_begin = container2.begin_side_cross_order();

        CHECK(it1_begin != it2_begin); // Different container instances, so iterators are unequal

        // Advance both iterators
        ++it1_begin;
        ++it2_begin;
        CHECK(it1_begin != it2_begin); // Still unequal as they refer to different containers

        // Iterators reaching their respective ends should also be unequal if from different containers
        MyContainer<int>::SideCrossOrderIterator it1_end = container1.end_side_cross_order();
        MyContainer<int>::SideCrossOrderIterator it2_end = container2.end_side_cross_order();
        CHECK(it1_end != it2_end); // End iterators from different containers are also unequal
    }

    SUBCASE("SideCrossOrderIterator comparison within the same container at different positions") {
        MyContainer<int> container;
        container.addElement(1);
        container.addElement(3);
        container.addElement(2);

        // Original: [1, 3, 2]
        // Sorted: [1, 2, 3]
        // SideCross: 1, 3, 2
        MyContainer<int>::SideCrossOrderIterator it1 = container.begin_side_cross_order(); // points to 1
        MyContainer<int>::SideCrossOrderIterator it2 = container.begin_side_cross_order(); // points to 1
        MyContainer<int>::SideCrossOrderIterator it3 = container.begin_side_cross_order();
        ++it3; // points to 3

        CHECK(it1 == it2); // Same logical position, same container -> equal
        CHECK_FALSE(it1 != it2);

        CHECK(it1 != it3); // Different logical positions, same container -> unequal
        CHECK_FALSE(it1 == it3);

        CHECK(it2 != it3);
        CHECK_FALSE(it2 == it3);
    }

    // Iterator Behavior with Container modifications 
    SUBCASE("SideCrossOrderIterator behavior after container modification (index snapshot)") {
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(30);
        container.addElement(20);
        CHECK(container.size() == 3); // Original elements: [10, 30, 20]

        // Iterator captures initial state's sorted indexes.
        // Based on original elements: [10, 20, 30] -> SideCross: 10, 30, 20
        MyContainer<int>::SideCrossOrderIterator it_side_cross_initial = container.begin_side_cross_order(); 

        // Verify initial iterator
        CHECK(*it_side_cross_initial == 10);
        ++it_side_cross_initial;
        CHECK(*it_side_cross_initial == 30);
        ++it_side_cross_initial;
        CHECK(*it_side_cross_initial == 20);
        ++it_side_cross_initial;
        CHECK(it_side_cross_initial == container.end_side_cross_order());


        // --- Modify the container ---
        // New container elements: [10, 30, 5] (after removal and addition)
        container.removeElement(20); 
        container.addElement(5);
        CHECK(container.size() == 3); 

        // New iterator created after modification, captures the *current* state.
        // Current container: [10, 30, 5]
        // Sorted: [5, 10, 30] -> SideCross: 5, 30, 10
        MyContainer<int>::SideCrossOrderIterator it_side_cross_after_mod = container.begin_side_cross_order(); 

        // Verify new iterator reflects the modified container's sorted order.
        CHECK(*it_side_cross_after_mod == 5);
        ++it_side_cross_after_mod; 
        CHECK(*it_side_cross_after_mod == 30); 
        ++it_side_cross_after_mod;
        CHECK(*it_side_cross_after_mod == 10); 
        ++it_side_cross_after_mod;
        CHECK(it_side_cross_after_mod == container.end_side_cross_order());
    }

    SUBCASE("Multiple SideCrossOrderIterators on the same container (independent snapshot of indexes)") {
        MyContainer<int> container;
        container.addElement(30);
        container.addElement(10);
        container.addElement(20);
        // Container elements initially: [30, 10, 20]

        // SideCrossOrderIterator creates an independent snapshot of sorted indexes
        // at the time of its construction.
        // Sorted order based on initial elements: [10, 20, 30] -> SideCross: 10, 30, 20
        MyContainer<int>::SideCrossOrderIterator it1 = container.begin_side_cross_order(); 
        MyContainer<int>::SideCrossOrderIterator it2 = container.begin_side_cross_order(); 

        // Although they are independent objects (each with their own sorted_original_indexes vector),
        // if created at the same logical position from the same container, their equality operator might return true.
        CHECK(it1 == it2); // Initially, both are at the same logical position (start)

        CHECK(*it1 == 10); // Both point to the first element in their respective side-cross sequence
        CHECK(*it2 == 10); 

        ++it1; // it1 advances to the next element in its side-cross sequence (30)
        CHECK(*it1 == 30);
        CHECK(*it2 == 10); // it2 should remain at 10

        CHECK(it1 != it2); // They are no longer at the same logical position

        ++it2; // it2 advances to the next element in its side-cross sequence (30)
        CHECK(it1 == it2); // Now they are at the same logical position again
        CHECK(*it1 == 30);
        CHECK(*it2 == 30);

        // Advance both to the end independently
        ++it1; // it1 is now at 20
        ++it1; // it1 is now at end()
        CHECK(it1 == container.end_side_cross_order()); // Check against an end iterator of the same container

        ++it2; // it2 is now at 20
        ++it2; // it2 is now at end()
        CHECK(it2 == container.end_side_cross_order()); // Check against an end iterator of the same container

        CHECK(it1 == it2); // Both are at their respective end positions
    }
}

TEST_CASE("MiddleOutOrderIterator operations") {

    // Basic Operations and Expected Order
    SUBCASE("Iterating through elements in middle-out order for various types") {
        MyContainer<int> intContainer;
        intContainer.addElement(7);
        intContainer.addElement(15); // Original index 1
        intContainer.addElement(6);  // Original index 2 (middle for size 5)
        intContainer.addElement(1);
        intContainer.addElement(2);

        // Original: [7, 15, 6, 1, 2]
        // MiddleOut Order: Middle, Left of Middle, Right of Middle, Next Left, Next Right
        // Expected: [6, 15, 1, 7, 2] 
        std::vector<int> expected_int_order = {6, 15, 1, 7, 2};
        std::vector<int> actual_int_order;

        for (auto it = intContainer.begin_middle_out_order(); it != intContainer.end_middle_out_order(); ++it) {
            actual_int_order.push_back(*it);
        }
        CHECK(actual_int_order == expected_int_order);

        MyContainer<std::string> stringContainer;
        stringContainer.addElement("cherry"); // Original index 0
        stringContainer.addElement("apple");  // Original index 1 (middle for size 3)
        stringContainer.addElement("banana"); // Original index 2

        // Original: ["cherry", "apple", "banana"]
        // MiddleOut Order: Middle, Left of Middle, Right of Middle
        // Expected: "apple", "cherry", "banana"
        std::vector<std::string> expected_string_order = {"apple", "cherry", "banana"};
        std::vector<std::string> actual_string_order;

        for (auto it = stringContainer.begin_middle_out_order(); it != stringContainer.end_middle_out_order(); ++it) {
            actual_string_order.push_back(*it);
        }
        CHECK(actual_string_order == expected_string_order);
    }

    SUBCASE("MiddleOutOrderIterator on a single element container") {
        MyContainer<double> container;
        container.addElement(3.14);

        MyContainer<double>::MiddleOutOrderIterator it = container.begin_middle_out_order();
        CHECK(*it == 3.14);
        
        ++it; // Advance to end
        CHECK(it == container.end_middle_out_order()); // Iterator should now be at the end
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing end() should throw
    }

    SUBCASE("Basic pre-increment and dereference for MiddleOutOrderIterator") {
        MyContainer<int> container;
        container.addElement(30); // Original index 0
        container.addElement(10); // Original index 1 (middle for size 3)
        container.addElement(20); // Original index 2

        // Original: [30, 10, 20]
        // MiddleOut: 10 (middle), 30 (left), 20 (right)
        // Expected: 10, 30, 20
        MyContainer<int>::MiddleOutOrderIterator it = container.begin_middle_out_order(); // Should point to 10
        CHECK(*it == 10); 
        
        ++it; // it now points to 30
        CHECK(*it == 30);

        ++it; // it now points to 20
        CHECK(*it == 20);

        ++it; // it now points to end_middle_out_order()
        CHECK(it == container.end_middle_out_order()); 
        CHECK_THROWS_AS(*it, std::out_of_range); // Attempt to dereference end()
    }

    SUBCASE("Basic post-increment behavior for MiddleOutOrderIterator") {
        MyContainer<char> container;
        container.addElement('c'); // Original index 0
        container.addElement('a'); // Original index 1 (middle for size 3)
        container.addElement('b'); // Original index 2

        // Original: ['c', 'a', 'b']
        // MiddleOut: 'a' (middle), 'c' (left), 'b' (right)
        // Expected: 'a', 'c', 'b'
        MyContainer<char>::MiddleOutOrderIterator it = container.begin_middle_out_order(); // 'it' initially points to 'a'

        MyContainer<char>::MiddleOutOrderIterator prev_it = it++; // 'prev_it' stores 'a', 'it' advances to 'c'

        CHECK(*prev_it == 'a'); // 'prev_it' should point to 'a'
        CHECK(*it == 'c'); // 'it' should point to 'c'

        MyContainer<char>::MiddleOutOrderIterator another_prev_it = it++; // 'another_prev_it' stores 'c', 'it' advances to 'b'
        
        CHECK(*another_prev_it == 'c'); // 'another_prev_it' should point to 'c'
        CHECK(*it == 'b'); // 'it' should point to 'b'

        MyContainer<char>::MiddleOutOrderIterator last_prev_it = it++; // 'last_prev_it' stores 'b', 'it' advances to end
        
        CHECK(*last_prev_it == 'b'); // 'last_prev_it' should point to 'b'
        CHECK(it == container.end_middle_out_order()); // 'it' should now be at end()
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing an iterator at or past end() should throw
    }

    // Edge Cases with Empty Container
    SUBCASE("MiddleOutOrderIterator on empty container") {
        MyContainer<int> container;

        // begin() should be equal to end() for an empty container
        MyContainer<int>::MiddleOutOrderIterator it_begin = container.begin_middle_out_order();
        MyContainer<int>::MiddleOutOrderIterator it_end = container.end_middle_out_order();
        CHECK(it_begin == it_end);
        CHECK_FALSE(it_begin != it_end);

        // Dereferencing begin() on an empty container should throw
        CHECK_THROWS_AS(*it_begin, std::out_of_range);
        // Dereferencing end() on an empty container should also throw
        CHECK_THROWS_AS(*it_end, std::out_of_range);

        // Incrementing an iterator on an empty container should still result in end()
        ++it_begin;
        CHECK(it_begin == it_end);
        CHECK_THROWS_AS(*it_begin, std::out_of_range); // Still throws on dereference
    }

    // Edge Cases with Iteration and Dereference
    SUBCASE("Dereferencing MiddleOutOrderIterator pointing to end() throws") {
        MyContainer<int> container;
        container.addElement(1);
        container.addElement(2);
        
        // Original: [1, 2]
        // MiddleOut: 1 (middle for even size, rounding down), 2 (right)
        // Expected: 1, 2
        MyContainer<int>::MiddleOutOrderIterator it_end = container.end_middle_out_order();
        CHECK_THROWS_AS(*it_end, std::out_of_range);

        MyContainer<int>::MiddleOutOrderIterator it = container.begin_middle_out_order(); // points to 1
        ++it; // points to 2
        ++it; // Now 'it' is at end()
        CHECK_THROWS_AS(*it, std::out_of_range);
    }

    SUBCASE("Pre-incrementing MiddleOutOrderIterator past end() does not throw but keeps it at end") {
        MyContainer<int> container;
        container.addElement(100);

        MyContainer<int>::MiddleOutOrderIterator it = container.begin_middle_out_order();
        ++it; // Now 'it' is at end()

        CHECK(it == container.end_middle_out_order()); 
        CHECK_THROWS_AS(*it, std::out_of_range); // Still throws on dereference

        ++it; // Incrementing past end()
        CHECK(it == container.end_middle_out_order()); // Should still be equal to end()
        CHECK_THROWS_AS(*it, std::out_of_range); // Should still throw on dereference
    }

    SUBCASE("Post-incrementing MiddleOutOrderIterator past end() does not throw but keeps it at end") {
        MyContainer<int> container;
        container.addElement(100);

        MyContainer<int>::MiddleOutOrderIterator it = container.begin_middle_out_order();
        MyContainer<int>::MiddleOutOrderIterator old_it = it++; // old_it is begin, it is end

        CHECK(*old_it == 100);
        CHECK(it == container.end_middle_out_order());
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing the advanced 'it' throws

        MyContainer<int>::MiddleOutOrderIterator old_it_2 = it++; // old_it_2 is end, it is still end
        CHECK(old_it_2 == container.end_middle_out_order());
        CHECK(it == container.end_middle_out_order());
        CHECK_THROWS_AS(*old_it_2, std::out_of_range); // Dereferencing old_it_2 throws
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing it throws
    }

    // Iterator Comparison Logic
    SUBCASE("MiddleOutOrderIterator comparison with different container instances") {
        MyContainer<int> container1;
        container1.addElement(30);
        container1.addElement(10);
        container1.addElement(20); 

        MyContainer<int> container2;
        container2.addElement(10);
        container2.addElement(30);
        container2.addElement(20); 

        // Iterators from different container instances should be considered unequal,
        // as they refer to different MyContainer objects.
        MyContainer<int>::MiddleOutOrderIterator it1_begin = container1.begin_middle_out_order();
        MyContainer<int>::MiddleOutOrderIterator it2_begin = container2.begin_middle_out_order();

        CHECK(it1_begin != it2_begin); // Different container instances, so iterators are unequal

        // Advance both iterators
        ++it1_begin;
        ++it2_begin;
        CHECK(it1_begin != it2_begin); // Still unequal as they refer to different containers

        // Iterators reaching their respective ends should also be unequal if from different containers
        MyContainer<int>::MiddleOutOrderIterator it1_end = container1.end_middle_out_order();
        MyContainer<int>::MiddleOutOrderIterator it2_end = container2.end_middle_out_order();
        CHECK(it1_end != it2_end); // End iterators from different containers are also unequal
    }

    SUBCASE("MiddleOutOrderIterator comparison within the same container at different positions") {
        MyContainer<int> container;
        container.addElement(1);
        container.addElement(3);
        container.addElement(2);

        // Original: [1, 3, 2]
        // MiddleOut: 3 (middle), 1 (left), 2 (right)
        // Expected: 3, 1, 2
        MyContainer<int>::MiddleOutOrderIterator it1 = container.begin_middle_out_order(); // points to 3
        MyContainer<int>::MiddleOutOrderIterator it2 = container.begin_middle_out_order(); // points to 3
        MyContainer<int>::MiddleOutOrderIterator it3 = container.begin_middle_out_order();
        ++it3; // points to 1

        CHECK(it1 == it2); // Same logical position, same container -> equal
        CHECK_FALSE(it1 != it2);

        CHECK(it1 != it3); // Different logical positions, same container -> unequal
        CHECK_FALSE(it1 == it3);

        CHECK(it2 != it3);
        CHECK_FALSE(it2 == it3);
    }

    // Iterator Behavior with Container modifications 
    SUBCASE("MiddleOutOrderIterator behavior after container modification (index snapshot)") {
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(30);
        container.addElement(20);
        CHECK(container.size() == 3); // Original elements: [10, 30, 20]

        // Iterator captures initial state's middle-out indexes.
        // Based on original elements: [10 (0), 30 (1), 20 (2)]
        // MiddleOut: 30 (middle index 1), 10 (left index 0), 20 (right index 2)
        // Expected: 30, 10, 20
        MyContainer<int>::MiddleOutOrderIterator it_middle_out_initial = container.begin_middle_out_order(); 

        // Verify initial iterator
        CHECK(*it_middle_out_initial == 30);
        ++it_middle_out_initial;
        CHECK(*it_middle_out_initial == 10);
        ++it_middle_out_initial;
        CHECK(*it_middle_out_initial == 20);
        ++it_middle_out_initial;
        CHECK(it_middle_out_initial == container.end_middle_out_order());

        // --- Modify the container ---
        container.removeElement(20); 
        container.addElement(5);
        CHECK(container.size() == 3); 
        // Current container elements: [10, 30, 5] (original indexes: 0:10, 1:30, 2:5)

        // New iterator created after modification, captures the *current* state.
        // Based on current elements: [10 (0), 30 (1), 5 (2)]
        // MiddleOut: 30 (middle index 1), 10 (left index 0), 5 (right index 2)
        // Expected: 30, 10, 5
        MyContainer<int>::MiddleOutOrderIterator it_middle_out_after_mod = container.begin_middle_out_order(); 

        // Verify new iterator reflects the modified container's middle-out order.
        CHECK(*it_middle_out_after_mod == 30);
        ++it_middle_out_after_mod; 
        CHECK(*it_middle_out_after_mod == 10); 
        ++it_middle_out_after_mod;
        CHECK(*it_middle_out_after_mod == 5); 
        ++it_middle_out_after_mod;
        CHECK(it_middle_out_after_mod == container.end_middle_out_order());
    }

    SUBCASE("Multiple MiddleOutOrderIterators on the same container (independent snapshot of indexes)") {
        MyContainer<int> container;
        container.addElement(30); // Original index 0
        container.addElement(10); // Original index 1 (middle for size 3)
        container.addElement(20); // Original index 2
        // Container elements initially: [30, 10, 20]

        // MiddleOutOrderIterator creates an independent snapshot of original indexes in middle-out order
        // MiddleOut order based on initial elements: 10, 30, 20
        MyContainer<int>::MiddleOutOrderIterator it1 = container.begin_middle_out_order(); 
        MyContainer<int>::MiddleOutOrderIterator it2 = container.begin_middle_out_order(); 

        // Although they are independent objects (each with their own arranged_original_indexes vector),
        // if created at the same logical position from the same container, their equality operator might return true.
        CHECK(it1 == it2); // Initially, both are at the same logical position (start)

        CHECK(*it1 == 10); // Both point to the first element in their respective middle-out sequence
        CHECK(*it2 == 10); 

        ++it1; // it1 advances to the next element in its middle-out sequence (30)
        CHECK(*it1 == 30);
        CHECK(*it2 == 10); // it2 should remain at 10

        CHECK(it1 != it2); // They are no longer at the same logical position

        ++it2; // it2 advances to the next element in its middle-out sequence (30)
        CHECK(it1 == it2); // Now they are at the same logical position again
        CHECK(*it1 == 30);
        CHECK(*it2 == 30);

        // Advance both to the end independently
        ++it1; // it1 is now at 20
        ++it1; // it1 is now at end()
        CHECK(it1 == container.end_middle_out_order()); // Check against an end iterator of the same container

        ++it2; // it2 is now at 20
        ++it2; // it2 is now at end()
        CHECK(it2 == container.end_middle_out_order()); // Check against an end iterator of the same container

        CHECK(it1 == it2); // Both are at their respective end positions
    }
}
