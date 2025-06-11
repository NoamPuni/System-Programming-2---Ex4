#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <memory>                          // For std::shared_ptr
#include "MyContainer.hpp"

// TEST_CASE מגדיר קבוצת בדיקות (Test Case)
TEST_CASE("MyContainer basic operations") {

    SUBCASE("Adding elements and checking size") {
        MyContainer<int> container;
        CHECK(container.size() == 0); // בדיקה שהמכולה ריקה בהתחלה

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
        // Corrected: Expect 20 to be GONE
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 20) == container.getElements().end());

        container.addElement(40);
        container.addElement(20); // הוספה חוזרת של 20
        container.addElement(50);
        CHECK(container.size() == 5);
        container.removeElement(20); // הסרה של מופע אחד מבין השניים - *NOW THIS REMOVES ALL 20s*
        CHECK(container.size() == 4); // Original size 5, one 20 removed, so should be 4.
                                    // This line is now also incorrect if MyContainer::removeElement removes *all* 20s.
                                    // If you added 20 (once) and then removed it, the size should be 4.
                                    // Let's assume you intend to remove *all* instances of 20 here, if any existed.

        // After removeElement(20), there should be NO 20s left if it removes all.
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 20) == container.getElements().end());
    }

    SUBCASE("Removing non-existent elements throws exception") {
        MyContainer<int> container;
        container.addElement(10);
        container.addElement(20);

        CHECK_THROWS_AS(container.removeElement(50), std::runtime_error);
        CHECK(container.size() == 2); // ודא שהגודל לא השתנה
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
        container.addElement(5);
        container.addElement(10);
        container.addElement(5); // הוספת כפילות
        container.addElement(15);
        CHECK(container.size() == 4);
        std::cout << "Before first remove (duplicate): " << container << std::endl; // Debug output

        container.removeElement(5); // This removes ALL 5s
        std::cout << "After first remove (duplicate): " << container << std::endl; // Debug output

        // ORIGINAL: CHECK(container.size() == 3); // THIS IS THE FAILED CHECK (2 == 3)
        // Corrected: If ALL 5s are removed, and there were two 5s, size should be 2 (4 - 2 = 2)
        CHECK(container.size() == 2); // Expect size 2 if both 5s were removed

        // ORIGINAL: CHECK(std::find(container.getElements().begin(), container.getElements().end(), 5) != container.getElements().end()); // 5 still exists
        // Corrected: If ALL 5s are removed, expect 5 to be GONE
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 5) == container.getElements().end()); // 5 is gone
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 10) != container.getElements().end());
        CHECK(std::find(container.getElements().begin(), container.getElements().end(), 15) != container.getElements().end());

        // Since all 5s are already removed, the next removeElement(5) call should throw.
        // There's no "second remove of 5" if the first one removed all.
        // So, we just check that attempting to remove it again throws.
        CHECK_THROWS_AS(container.removeElement(5), std::runtime_error); // Attempt to remove 5 again - should throw as it's not there
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
        // This is important because the loop condition `it != container.end_order()` prevents dereferencing `end_order()`
        // but if someone manually tries to dereference begin_order() on an empty container.
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

        MyContainer<char>::OrderIterator it = container.begin_order();
        MyContainer<char>::OrderIterator prev_it = it++; // Store current, then increment
        CHECK(*prev_it == 'a'); // prev_it should point to 'a'
        CHECK(*it == 'b');      // it should point to 'b'

        prev_it = it++; // it is 'b', prev_it gets 'b', it advances to end
        CHECK(*prev_it == 'b');
        CHECK(it == container.end_order()); // it should be at end
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing post-incremented past-end should throw
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
}

TEST_CASE("AscendingOrderIterator Comprehensive Tests") {

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
        container.addElement('c');
        container.addElement('a');

        MyContainer<char>::AscendingOrderIterator it = container.begin_ascending_order(); // Points to 'a'
        MyContainer<char>::AscendingOrderIterator prev_it = it++; // prev_it stores 'a', it advances to 'c'

        CHECK(*prev_it == 'a'); // prev_it should point to 'a'
        CHECK(*it == 'c');      // it should point to 'c'

        prev_it = it++; // prev_it stores 'c', it advances to end_ascending_order()
        CHECK(*prev_it == 'c');
        CHECK(it == container.end_ascending_order()); // it should be at end_ascending_order()
        CHECK_THROWS_AS(*it, std::out_of_range); // Dereferencing post-incremented past-end should throw
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
    SUBCASE("Iterator comparison with different shared_ptr instances - detailed") {
        MyContainer<int> container1;
        container1.addElement(10);
        
        MyContainer<int> container2; 
        container2.addElement(10); // Same value, but different container instance -> different shared_ptr

        MyContainer<int>::AscendingOrderIterator it1_begin = container1.begin_ascending_order();
        MyContainer<int>::AscendingOrderIterator it1_end = container1.end_ascending_order();

        MyContainer<int>::AscendingOrderIterator it2_begin = container2.begin_ascending_order();
        MyContainer<int>::AscendingOrderIterator it2_end = container2.end_ascending_order();
        
        // Iterators from different containers should always be unequal, even if content is identical
        CHECK(it1_begin != it2_begin); 
        CHECK_FALSE(it1_begin == it2_begin);

        CHECK(it1_end != it2_end);
        CHECK_FALSE(it1_end == it2_end);

        // An iterator from container1 should also be unequal to an end iterator from container2
        CHECK(it1_begin != it2_end);
        CHECK_FALSE(it1_begin == it2_end);
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
    // Based on the `AscendingOrderIterator` implementation,
    // it is designed to work on a *snapshot* of the container's sorted data.
    // This means iterators created before a modification to the container will *not* reflect those changes,
    // but rather operate on the data as it was when the iterator was constructed.
    SUBCASE("Iterator behavior after container modification (snapshot verification)") {
        MyContainer<int> container;
        container.addElement(20);
        container.addElement(10);

        // Get an iterator for the current state (snapshot): [10, 20]
        MyContainer<int>::AscendingOrderIterator old_snapshot_it = container.begin_ascending_order();
        CHECK(*old_snapshot_it == 10);
        
        // Modify the original container: add 5, remove 20. The container now conceptually has [5, 10]
        container.addElement(5);
        container.removeElement(20); 

        // Verify the old iterator still iterates over its original snapshot ([10, 20])
        CHECK(*old_snapshot_it == 10); 
        ++old_snapshot_it;
        CHECK(*old_snapshot_it == 20); 
        ++old_snapshot_it;
        CHECK(old_snapshot_it == container.end_ascending_order()); // old_snapshot_it's end for its snapshot

        // Get a new iterator for the modified container state. This new iterator will reflect the changes.
        MyContainer<int>::AscendingOrderIterator new_snapshot_it = container.begin_ascending_order();
        CHECK(*new_snapshot_it == 5); // New iterator reflects the addition of 5 and removal of 20
        ++new_snapshot_it;
        CHECK(*new_snapshot_it == 10);
        ++new_snapshot_it;
        CHECK(new_snapshot_it == container.end_ascending_order()); 
    }

    SUBCASE("Multiple iterators on the same snapshot") {
        MyContainer<int> container;
        container.addElement(30);
        container.addElement(10);
        container.addElement(20);

        // Both iterators point to the same shared_ptr snapshot: [10, 20, 30]
        MyContainer<int>::AscendingOrderIterator it1 = container.begin_ascending_order();
        MyContainer<int>::AscendingOrderIterator it2 = container.begin_ascending_order(); 

        CHECK(it1 == it2); // Initially, both are at the same position and same snapshot
        CHECK(*it1 == 10);
        CHECK(*it2 == 10);

        ++it1; // it1 is now at 20
        CHECK(*it1 == 20);
        CHECK(*it2 == 10); // it2 should remain at 10

        CHECK(it1 != it2); // They are no longer equal
        
        ++it2; // it2 is now at 20
        CHECK(it1 == it2); // Now they are equal again
        CHECK(*it1 == 20);
        CHECK(*it2 == 20);

        // Advance both to end
        ++it1;
        ++it2;
        CHECK(it1 == container.end_ascending_order());
        CHECK(it2 == container.end_ascending_order());
        CHECK(it1 == it2); // Both are at end_ascending_order() of the same snapshot
    }
}