//MyContainer.hpp
#include <vector>
#include <iostream>
#include <algorithm> // For std::sort, std::remove
#include <stdexcept> // For std::out_of_range, std::runtime_error
#include <string>    // Included for string tests if needed
#include <memory>    // For std::shared_ptr

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
        auto it = std::remove(elements.begin(), elements.end(), element);
        if (it == elements.end()) {
            throw std::runtime_error("Element not found in container.");
        }
        elements.erase(it, elements.end());
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
    
    //template <typename T> // The iterator is templated on the same type as MyContainer
    class AscendingOrderIterator { // Specifies that AscendingOrderIterator is nested within MyContainer
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

    // --- Other iterator types would be implemented similarly ---
    // For example:
    // DescendingOrderIterator begin_descending_order() const;
    // DescendingOrderIterator end_descending_order() const;
    // ReverseOrderIterator begin_reverse_order() const;
    // ReverseOrderIterator end_reverse_order() const;

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