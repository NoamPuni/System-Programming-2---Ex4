//MyContainer.hpp
#include <vector>
#include <iostream>
#include <algorithm> // For std::sort, std::remove
#include <stdexcept> // For std::out_of_range, std::runtime_error
#include <string>    // Included for string tests if needed

namespace Container {
    template <typename T>
    class MyContainer {
    private:
        std::vector<T> elements;

    public:
        MyContainer() = default;

        void addElement(const T& element) {
            elements.push_back(element);
        }

        void removeElement(const T& element) {
            auto original_size = elements.size();
            auto it = std::remove(elements.begin(), elements.end(), element);
            elements.erase(it, elements.end());
            
            if (elements.size() == original_size) {
                throw std::runtime_error("Element not found in container.");
            }
        }

        size_t size() const {
            return elements.size();
        }

        const std::vector<T>& getElements() const {
            return elements;
        }

        template <typename U>
        friend std::ostream& operator<<(std::ostream& os, const MyContainer<U>& container);

        // --- 1. OrderIterator (Iterates in insertion order)
        class OrderIterator {
        private:
            // the iterator holds a valid reference to the parent container,
            // which remains valid even if the underlying 'elements' vector reallocates.
            const MyContainer<T>& cont;
            size_t current_index;

        public:
            // Constructor for OrderIterator.
            // Initializes the iterator with a reference to the container and a starting index.
            OrderIterator(const MyContainer<T>& c, size_t index)
                : cont(c), current_index(index) {}
            
            // Dereference operator (*it).
            // Provides access to the element currently pointed to by the iterator.
            const T& operator*() const {
                // Access elements through the container's getElements() method,
                // ensuring access to the current, valid underlying vector.
                if (current_index >= cont.size()) {
                    throw std::out_of_range("OrderIterator: Dereference out of bounds.");
                }
                return cont.getElements()[current_index];
            }

            // Pre-increment operator (++it).
            // Advances the iterator to the next element in the container.
            OrderIterator& operator++() {
                // No bounds check here for increment, as standard iterators can be incremented to 'end'.
                // Dereference operator handles out-of-bounds access.
                current_index++;
                return *this;
            }
            // Post-increment operator (it++).
            // Advances the iterator to the next element, but returns a copy of the iterator's state before the increment.  
            OrderIterator operator++(int) {
                OrderIterator temp = *this;
                ++(*this);
                return temp;
            }

            // Equality operator (it1 == it2).
            // Compares two OrderIterator objects for equality.
            bool operator==(const OrderIterator& other) const {
                // Equality check now compares the container references, ensuring both iterators
                // belong to the same logical container instance.
                return current_index == other.current_index && &cont == &other.cont;
            }
            // Inequality operator (it1 != it2).
            // Compares two OrderIterator objects for inequality.
            bool operator!=(const OrderIterator& other) const {
                return !(*this == other);
            }
        };

        // Begin and end methods for OrderIterator.
        OrderIterator begin_order() const {
            return OrderIterator(*this, 0);
        }

        OrderIterator end_order() const {
            return OrderIterator(*this, elements.size());
        }

        // --- 2. AscendingOrderIterator (sorted from smallest to largest); 
        class AscendingOrderIterator { 
        private:
            // A constant reference to the parent MyContainer instance.
            // This allows the iterator to access the container's elements via getElements().
            const MyContainer<T>& cont;

            // A vector to store the indexes of the original elements, sorted according to their values.
            // This forms the "snapshot" for this specific iterator.
            std::vector<size_t> indexes;

            // The current position within the `indexes` vector during iteration.
            size_t current_index_in_sorted_indexes;

        public:
            
            // Constructor for AscendingOrderIterator.
            // Initializes the iterator by sorting indexes based on the container's elements.
            // This constructor performs a sort operation each time it's called.
            // is_end_iterator_flag: true if this is an end iterator, false for begin.
            AscendingOrderIterator(const MyContainer<T>& cont, bool is_end_iterator_flag)
                : cont(cont) { // current_index_in_sorted_indexes will be set later

                // Populate 'indexes' with initial order (0 to size-1)
                for (size_t i = 0; i < cont.getElements().size(); ++i) {
                    indexes.push_back(i);
                }

                // Sort 'indexes' based on the values in the container (lambda function).
                // This lambda captures the container reference and sorts indexes based on the elements.
                std::sort(indexes.begin(), indexes.end(),
                    [&](size_t a, size_t b) {
                        // Ensure sorting is stable and handles equal elements correctly if needed
                        return cont.getElements()[a] < cont.getElements()[b];
                    });

                if (is_end_iterator_flag) { // For end iterator
                    current_index_in_sorted_indexes = indexes.size();
                } else { // For begin iterator
                    current_index_in_sorted_indexes = 0;
                }
            }

            //Dereference operator (*it).
            //Provides access to the element currently pointed to by the iterator.
            const T& operator*() const {
                // Ensure the current index is within valid bounds of the sorted indexes.
                if (current_index_in_sorted_indexes >= indexes.size()) {
                    throw std::out_of_range("AscendingOrderIterator: Dereference out of bounds.");
                }
                // Use the current sorted index to access the actual element from the MyContainer.
                return cont.getElements()[indexes[current_index_in_sorted_indexes]];
            }

            //Pre-increment operator (++it).
            //Advances the iterator to the next element in the sorted sequence.
            AscendingOrderIterator& operator++() {
                if (current_index_in_sorted_indexes < indexes.size()) {
                    current_index_in_sorted_indexes++; // Move to the next index in our sorted list
                } 
                return *this;
            }

            //Post-increment operator (it++).
            //Advances the iterator to the next element, but returns a copy of the iterator's state
            // *before* the increment.
            AscendingOrderIterator operator++(int) {
                AscendingOrderIterator temp = *this; // Save current state
                ++(*this); // Increment actual iterator
                return temp; // Return saved state
            }

            //Equality operator (it1 == it2).
            //Compares two AscendingOrderIterator objects for equality.
            bool operator==(const AscendingOrderIterator& other) const {
                // Iterators are equal if their internal index is the same AND they refer to the same container instance.
                return current_index_in_sorted_indexes == other.current_index_in_sorted_indexes && &cont == &other.cont;
            }

            //Inequality operator (it1 != it2).
            //Compares two AscendingOrderIterator objects for inequality.
            bool operator!=(const AscendingOrderIterator& other) const {
                return !(*this == other);
            }
            
        };

        // Begin and end methods for AscendingOrderIterator.
        AscendingOrderIterator begin_ascending_order() const {
            return AscendingOrderIterator(*this, false); // false indicates this is a begin iterator
        }
        
        AscendingOrderIterator end_ascending_order() const {
            return AscendingOrderIterator(*this, true); // true indicates this is an end iterator
        }

        // --- 3. DescendingOrderIterator (sorted from largest to smallest)
        class DescendingOrderIterator { 
        private:
            // A constant reference to the parent MyContainer instance.
            // This allows the iterator to access the container's elements via getElements().
            const MyContainer<T>& cont;

            // A vector to store the indexes of the original elements, sorted according to their values.
            // This forms the "snapshot" for this specific iterator.
            std::vector<size_t> indexes;

            // The current position within the `indexes` vector during iteration.
            size_t current_index_in_sorted_indexes;

        public:
            // Constructor for DescendingOrderIterator.
            // Initializes the iterator by sorting indexes based on the container's elements in descending order.
            // This constructor performs a sort operation each time it's called.
            // is_end_iterator_flag: true if this is an end iterator, false for begin.
            DescendingOrderIterator(const MyContainer<T>& cont, bool is_end_iterator_flag)
                : cont(cont) {

                // Populate 'indexes' with initial order (0 to size-1)
                for (size_t i = 0; i < cont.size(); ++i) {
                    indexes.push_back(i);
                }

                // Sort 'indexes' based on the values in the container (lambda function).
                // This lambda captures the container reference and sorts indexes based on the elements.
                // Sorts in DESCENDING order (a > b)
                std::sort(indexes.begin(), indexes.end(),
                    [&](size_t a, size_t b) {
                        // Ensures sorting is stable and handles equal elements correctly if needed
                        return cont.getElements()[a] > cont.getElements()[b]; 
                        //same as it was implemented earlier, changed from < to > for descending order
                    });

                if (is_end_iterator_flag) { // For end iterator
                    current_index_in_sorted_indexes = indexes.size();
                } else { // For begin iterator
                    current_index_in_sorted_indexes = 0;
                }
            }

            //Dereference operator (*it).
            //Provides access to the element currently pointed to by the iterator.
            const T& operator*() const {
                // Ensure the current index is within valid bounds of the sorted indexes.
                if (current_index_in_sorted_indexes >= indexes.size()) {
                    throw std::out_of_range("DescendingOrderIterator: Dereference out of bounds."); // Updated message
                }
                // Use the current sorted index to access the actual element from the MyContainer.
                return cont.getElements()[indexes[current_index_in_sorted_indexes]];
            }

            //Pre-increment operator (++it).
            //Advances the iterator to the next element in the sorted sequence.
            DescendingOrderIterator& operator++() {
                if (current_index_in_sorted_indexes < indexes.size()) {
                    current_index_in_sorted_indexes++; // Move to the next index in our sorted list
                } 
                return *this;
            }

            //Post-increment operator (it++).
            //Advances the iterator to the next element, but returns a copy of the iterator's state
            // *before* the increment.
            DescendingOrderIterator operator++(int) {
                DescendingOrderIterator temp = *this; // Save current state
                ++(*this); // Increment actual iterator
                return temp; // Return saved state
            }

            //Equality operator (it1 == it2).
            //Compares two DescendingOrderIterator objects for equality.
            bool operator==(const DescendingOrderIterator& other) const {
                // Iterators are equal if their internal index is the same AND they refer to the same container instance.
                return current_index_in_sorted_indexes == other.current_index_in_sorted_indexes && &cont == &other.cont;
            }

            //Inequality operator (it1 != it2).
            //Compares two DescendingOrderIterator objects for inequality.
            bool operator!=(const DescendingOrderIterator& other) const {
                return !(*this == other);
            }
        };

        // Begin and end methods for DescendingOrderIterator.
        DescendingOrderIterator begin_descending_order() const {
            return DescendingOrderIterator(*this, false); // false indicates this is a begin iterator
        }
        
        DescendingOrderIterator end_descending_order() const {
            return DescendingOrderIterator(*this, true); // true indicates this is an end iterator
        }

        // --- 4. ReverseOrderIterator (Iterates in reverse order of insertion)
        class ReverseOrderIterator { 
        private:
            // the iterator holds a valid reference to the parent container,
            // which remains valid even if the underlying 'elements' vector reallocates.
            const MyContainer<T>& cont;
            size_t current_index;

        public:
            // Constructor for ReverseOrderIterator.
            // Initializes the iterator with a reference to the container and a starting index.
            ReverseOrderIterator(const MyContainer<T>& c, size_t index) 
                : cont(c), current_index(index) {}
            
            // Dereference operator (*it).
            // Provides access to the element currently pointed to by the iterator.
            const T& operator*() const {
                // Access elements through the container's getElements() method,
                // ensuring access to the current, valid underlying vector.
                if (current_index >= cont.size()) {
                    throw std::out_of_range("ReverseOrderIterator: Dereference out of bounds."); 
                }
                return cont.getElements()[current_index];
            }

            // Pre-increment operator (++it).
            // Advances the iterator to the next element in the container.
            ReverseOrderIterator& operator++() { 
                // No bounds check here for increment, as standard iterators can be incremented to 'end'.
                // Dereference operator handles out-of-bounds access.
                current_index--; // Changed operation from ++ to --
                return *this;
            }
            // Post-increment operator (it++).
            // Advances the iterator to the next element, but returns a copy of the iterator's state before the increment.
            ReverseOrderIterator operator++(int) {
                ReverseOrderIterator temp = *this;
                ++(*this);
                return temp;
            }

            // Equality operator (it1 == it2).
            // Compares two ReverseOrderIterator objects for equality.
            bool operator==(const ReverseOrderIterator& other) const { 
                // Equality check now compares the container references, ensuring both iterators
                // belong to the same logical container instance.
                return current_index == other.current_index && &cont == &other.cont;
            }
            // Inequality operator (it1 != it2).
            // Compares two ReverseOrderIterator objects for inequality.
            bool operator!=(const ReverseOrderIterator& other) const { 
                return !(*this == other);
            }
        };

        // Begin and end methods for ReverseOrderIterator.
        ReverseOrderIterator begin_reverse_order() const { 
            // For reverse iteration, begin is the last element.
            if (elements.empty()) {
                return ReverseOrderIterator(*this, static_cast<size_t>(-1));// Return an end iterator
            }
            return ReverseOrderIterator(*this, elements.size() - 1);
        }
        
        ReverseOrderIterator end_reverse_order() const {
            // The iterator will start from the last element and move backwards.
            // The iterator will stop exactly when it has moved past the first element.
            // For reverse iteration, end is conceptually "before" the first element (index [-1]).
            // Using static_cast<size_t>(-1) is to handle the end of a reverse iteration
            return ReverseOrderIterator(*this, static_cast<size_t>(-1));
        }

        // --- 5. SideCrossOrderIterator (Iterates in side-cross order: smallest, largest, second-smallest, second-largest, etc.)
        class SideCrossOrderIterator {
        private:
            // A constant reference to the parent MyContainer instance.
            // This allows the iterator to access the container's elements via getElements().
            const MyContainer<T>& cont;

            // A vector to store the indexes of the original elements, sorted according to their values.
            // This forms the "snapshot" for this specific iterator's traversal logic.
            std::vector<size_t> sorted_original_indexes;

            // Pointers to the current smallest and largest elements in the sorted_original_indexes list.
            size_t left_ptr_in_sorted_indexes;  // Points to the current smallest remaining element
            size_t right_ptr_in_sorted_indexes; // Points to the current largest remaining element

            // Flag to determine whether to take the next element from the left (smallest) or right (largest) side.
            bool is_left_turn;

            // The actual index in the *original* container that the iterator currently points to.
            // This is what operator* will dereference. For the end iterator, this will be sorted_original_indexes.size().
            size_t current_returned_original_index;
            public:
            // Constructor for SideCrossOrderIterator.
            // Initializes the iterator by sorting indexes based on the container's elements
            // and setting up the initial pointers for side-cross traversal.
            // is_end_iterator_flag: true if this is an end iterator, false for begin.
            SideCrossOrderIterator(const MyContainer<T>& c, bool is_end_iterator_flag)
                : cont(c) {
                // Populate 'sorted_original_indexes' with initial order (0 to size-1)
                for (size_t i = 0; i < cont.getElements().size(); ++i) {
                    sorted_original_indexes.push_back(i);
                }

                // Sort 'sorted_original_indexes' based on the values in the container.
                // This lambda captures the container reference and sorts indexes based on the elements.
                std::sort(sorted_original_indexes.begin(), sorted_original_indexes.end(),
                    [&](size_t a, size_t b) {
                        return cont.getElements()[a] < cont.getElements()[b];
                    });

                // Initialize left and right pointers for traversal
                left_ptr_in_sorted_indexes = 0;
                right_ptr_in_sorted_indexes = sorted_original_indexes.size() - 1;
                is_left_turn = true; // Start with the smallest element

                if (is_end_iterator_flag || sorted_original_indexes.empty()) {
                    // For the end iterator, or an empty container, set the current index to past-the-end.
                    current_returned_original_index = sorted_original_indexes.size();
                    // Ensure the bounds are in an "ended" state for consistency.
                    left_ptr_in_sorted_indexes = sorted_original_indexes.size();
                    right_ptr_in_sorted_indexes = sorted_original_indexes.size() -1; // Makes left_ptr > right_ptr
                } else { // For begin iterator on a non-empty container
                    // Set the initial element to the smallest (first in sorted list)
                    current_returned_original_index = sorted_original_indexes[left_ptr_in_sorted_indexes];
                }
            }

            //Dereference operator (*it).
            //Provides access to the element currently pointed to by the iterator.
            const T& operator*() const {
                // Ensure the current index is within valid bounds of the original container.
                if (current_returned_original_index >= cont.size()) {
                    throw std::out_of_range("SideCrossOrderIterator: Dereference out of bounds.");
                }
                // Use the stored original index to access the actual element from the MyContainer.
                return cont.getElements()[current_returned_original_index];
            }
            //Pre-increment operator (++it).
            //Advances the iterator to the next element in the side-cross sequence.
            SideCrossOrderIterator& operator++() {
                // If already at the end, do nothing.
                if (current_returned_original_index == sorted_original_indexes.size()) {
                    return *this;
                }

                // Advance pointers based on current turn
                if (is_left_turn) {
                    left_ptr_in_sorted_indexes++;
                    is_left_turn = false;
                } else {
                    right_ptr_in_sorted_indexes--;
                    is_left_turn = true;
                }

                // Update current_returned_original_index based on the new pointer positions
                if (left_ptr_in_sorted_indexes <= right_ptr_in_sorted_indexes) {
                    if (is_left_turn) { // Next will be from left side
                        current_returned_original_index = sorted_original_indexes[left_ptr_in_sorted_indexes];
                    } else { // Next will be from right side
                        current_returned_original_index = sorted_original_indexes[right_ptr_in_sorted_indexes];
                    }
                } else {
                    // All elements have been processed, set to end state
                    current_returned_original_index = sorted_original_indexes.size();
                }
                
                return *this;
            }

            //Post-increment operator (it++).
            //Advances the iterator to the next element, but returns a copy of the iterator's state
            // *before* the increment.
            SideCrossOrderIterator operator++(int) {
                SideCrossOrderIterator temp = *this; // Save current state
                ++(*this); // Increment actual iterator
                return temp; // Return saved state
            }

            //Equality operator (it1 == it2).
            //Compares two SideCrossOrderIterator objects for equality.
            bool operator==(const SideCrossOrderIterator& other) const {
                // Iterators are equal if their currently pointed-to original index is the same
                // AND they refer to the same container instance.
                return current_returned_original_index == other.current_returned_original_index && &cont == &other.cont;
            }

            //Inequality operator (it1 != it2).
            //Compares two SideCrossOrderIterator objects for inequality.
            bool operator!=(const SideCrossOrderIterator& other) const {
                return !(*this == other);
            }
        };

        // Begin and end methods for SideCrossOrderIterator.
        SideCrossOrderIterator begin_side_cross_order() const {
            return SideCrossOrderIterator(*this, false); // false indicates this is a begin iterator
        }

        SideCrossOrderIterator end_side_cross_order() const {
            return SideCrossOrderIterator(*this, true); // true indicates this is an end iterator
        }

        // --- 6. MiddleOutOrderIterator (Iterates from the middle element outwards, alternating left and right) 
        class MiddleOutOrderIterator {
        private:
            // A constant reference to the parent MyContainer instance.
            // This allows the iterator to access the container's elements via getElements().
            const MyContainer<T>& cont;

            // A vector to store the indexes of the original elements, arranged in middle-out order.
            // This forms the "snapshot" of the traversal path for this specific iterator.
            std::vector<size_t> arranged_original_indexes;

            // The current position within the `arranged_original_indexes` vector during iteration.
            size_t current_index_in_arranged_indexes;
        public:
            // Constructor for MiddleOutOrderIterator.
            // Initializes the iterator by arranging the original indexes in middle-out order.
            // This constructor performs the arrangement operation each time it's called.
            // is_end_iterator_flag: true if this is an end iterator, false for begin.
            MiddleOutOrderIterator(const MyContainer<T>& c, bool is_end_iterator_flag)
                : cont(c) {
                
                size_t n = cont.getElements().size();

                if (n == 0) {
                    // For an empty container, the arranged_original_indexes remains empty.
                    current_index_in_arranged_indexes = 0; // Or any value that equals arranged_original_indexes.size() (which is 0)
                    return;
                }

                // Calculate the middle index (or indices for even count).
                // using floor division for the middle index, which rounds down.
                // For [odd-sized] (e.g size 5), middle_index_base = (5-1)/2 = index 2.
                // For [even-sized] (e.g size 4), middle_index_base = (4-1)/2 = index 1.
                int middle_index_base = (n - 1) / 2;

                // Add the middle element first
                arranged_original_indexes.push_back(middle_index_base);

                // Pointers for outward expansion
                int left_offset = 1;  // Distance to the left from middle_index_base
                int right_offset = 1; // Distance to the right from middle_index_base
                bool use_left = true; // Flag to alternate between left and right

                // Continue adding elements until all are included
                while (arranged_original_indexes.size() < n) {
                    if (use_left) {
                        int left_index = middle_index_base - left_offset;
                        if (left_index >= 0) { // Check if left index is valid
                            arranged_original_indexes.push_back(left_index);
                        }
                        left_offset++;
                    } else {
                        int right_index = middle_index_base + right_offset;
                        if (right_index < n) { // Check if right index is valid
                            arranged_original_indexes.push_back(right_index);
                        }
                        right_offset++;
                    }
                    use_left = !use_left; // Toggle for next iteration
                }

                // Set current_index_in_arranged_indexes based on whether it's a begin or end iterator
                if (is_end_iterator_flag) {
                    current_index_in_arranged_indexes = arranged_original_indexes.size();
                } else {
                    current_index_in_arranged_indexes = 0;
                }
            }
            
            // Dereference operator (*it).
            // Provides access to the element currently pointed to by the iterator.
            const T& operator*() const {
                // Ensure the current index is within valid bounds of the arranged indexes.
                if (current_index_in_arranged_indexes >= arranged_original_indexes.size()) {
                    throw std::out_of_range("MiddleOutOrderIterator: Dereference out of bounds.");
                }
                // Use the current arranged index to access the actual element from the MyContainer.
                return cont.getElements()[arranged_original_indexes[current_index_in_arranged_indexes]];
            }

            // Pre-increment operator (++it).
            // Advances the iterator to the next element in the middle-out sequence.
            MiddleOutOrderIterator& operator++() {
                if (current_index_in_arranged_indexes < arranged_original_indexes.size()) {
                    current_index_in_arranged_indexes++; // Move to the next index in our arranged list
                } 
                return *this;
            }

            // Post-increment operator (it++).
            // Advances the iterator to the next element, but returns a copy of the iterator's state
            // *before* the increment.
            MiddleOutOrderIterator operator++(int) {
                MiddleOutOrderIterator temp = *this; // Save current state
                ++(*this); // Increment actual iterator
                return temp; // Return saved state
            }

            // Equality operator (it1 == it2).
            // Compares two MiddleOutOrderIterator objects for equality.
            bool operator==(const MiddleOutOrderIterator& other) const {
                // Iterators are equal if their internal index is the same AND they refer to the same container instance.
                return current_index_in_arranged_indexes == other.current_index_in_arranged_indexes && &cont == &other.cont;
            }

            // Inequality operator (it1 != it2).
            // Compares two MiddleOutOrderIterator objects for inequality.
            bool operator!=(const MiddleOutOrderIterator& other) const {
                return !(*this == other);
            }
        };

        // Begin and end methods for MiddleOutOrderIterator.
        MiddleOutOrderIterator begin_middle_out_order() const {
            return MiddleOutOrderIterator(*this, false); // false indicates this is a begin iterator
        }

        MiddleOutOrderIterator end_middle_out_order() const {
            return MiddleOutOrderIterator(*this, true); // true indicates this is an end iterator
        }
        
        // Global operator<< for MyContainer for easy printing.
        friend std::ostream& operator<<(std::ostream& os, const MyContainer<T>& container) {
            os << "MyContainer elements: [";
            for (size_t i = 0; i < container.elements.size(); ++i) {
                os << container.elements[i];
                if (i < container.elements.size() - 1) {
                    os << ", ";
                }
            }
            os << "]";
            return os;
        }
    };//end of MyContainer class
}
