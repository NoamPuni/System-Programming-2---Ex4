# System-Programming-2---Ex4

### MyContainer - C++ Data Container with Custom Iterators

## Introduction

This project implements a generic container (`MyContainer`) in C++ capable of storing elements of any data type. The unique aspect of this container lies in its ability to provide multiple custom iterators, allowing elements to be traversed in various orders beyond the standard insertion order.

The project is developed using Object-Oriented Programming principles and advanced C++ features such as Templates, Nested Classes, and Operator Overloading.

## Project Structure

The project consists of three main files:

* **`MyContainer.hpp`**: A header file containing the definition of the `MyContainer` class and all its nested iterator classes.
* **`Test.cpp`**: A file containing unit tests for the `MyContainer` class and all its iterators, utilizing the `doctest` framework.
* **`main.cpp`**: A simple demonstration file that showcases the usage of the container and its various iterators by printing output to the console.

### `MyContainer.hpp` - The Container Class and Its Iterators

This file defines the `MyContainer<T>` template class, which includes:
* **`std::vector<T> elements`**: A private vector for storing the actual elements.
* **Basic methods**: `addElement`, `removeElement`, `size`, `getElements`.
* **`operator<<`**: A global friend function enabling convenient printing of the container's contents.

Additionally, `MyContainer.hpp` defines **six nested iterator classes**, each with unique traversal logic:

1.  **`OrderIterator`**:
    * **Traversal Order**: Elements are traversed in their original insertion order (left to right).
    * **Example**: For `[7,15,6,1,2]`, the order will be `7,15,6,1,2`.
    * **Implementation**: Maintains a pointer to the current index in the original elements vector.

2.  **`AscendingOrderIterator`**:
    * **Traversal Order**: Elements are traversed in ascending order (from smallest to largest).
    * **Example**: For `[7,15,6,1,2]`, the order will be `1,2,6,7,15`.
    * **Implementation**: Constructs a "snapshot" of the original indices sorted by the elements' values in its constructor, then iterates over these sorted indices.

3.  **`DescendingOrderIterator`**:
    * **Traversal Order**: Elements are traversed in descending order (from largest to smallest).
    * **Example**: For `[7,15,6,1,2]`, the order will be `15,7,6,2,1`.
    * **Implementation**: Similar to `AscendingOrderIterator`, but sorts in descending order.

4.  **`ReverseOrderIterator`**:
    * **Traversal Order**: Elements are traversed in reverse of their insertion order (right to left).
    * **Example**: For `[7,15,6,1,2]`, the order will be `2,1,6,15,7`.
    * **Implementation**: Maintains a pointer to the current index in the original elements vector and moves backward.

5.  **`SideCrossOrderIterator`**:
    * **Traversal Order**: Alternates between the smallest and largest available elements in the sorted sequence. It takes the smallest, then the largest, then the second smallest, then the second largest, and so on.
    * **Example**: For `[7,15,6,1,2]`, the order will be `1,15,2,7,6`.
    * **Implementation**: Builds a "snapshot" of the original indices sorted by value. It then traverses using two pointers (one from the start, one from the end of the sorted indices) alternating between them.

6.  **`MiddleOutOrderIterator`**:
    * **Traversal Order**: Starts with the middle element (based on original index), then alternates between elements to its left and right, moving outwards.
    * **Example**: For `[7,15,6,1,2]`, the order will be `6,15,1,7,2`.
    * **Implementation**: Constructs a "snapshot" of original indices arranged in "middle-out" order within its constructor, then iterates linearly over this arranged list of indices.

Each iterator class implements the standard iterator operators:
* `operator*()`: Dereference to access the current element.
* `operator++()`: Pre-increment to advance to the next element.
* `operator++(int)`: Post-increment to advance, returning a copy of the iterator's state before increment.
* `operator==()`: Equality comparison between iterators.
* `operator!=()`: Inequality comparison between iterators.

Additionally, the `MyContainer` class provides `begin_X_order()` and `end_X_order()` methods for each iterator type, allowing for convenient traversal initiation and termination.

### `Test.cpp` - Unit Tests

This file contains comprehensive tests using the `doctest` framework to ensure the correctness and robustness of the `MyContainer` class and all its iterators.
The tests cover:
* Basic container operations (add, remove, size).
* Correctness of the traversal order for each iterator across various data types and container sizes.
* Iterator behavior on an empty container.
* Behavior of `++` (pre and post) and `*` operators.
* Exception handling (`std::out_of_range`) when attempting to dereference out-of-bounds iterators.
* Comparison of iterators from the same and different container instances.
* Ensuring that iterators capture a "snapshot" of the traversal order at their creation time and are not affected by subsequent modifications to the container (except for changes to the elements themselves if they were previously referenced by index).

To run the tests, `Test.cpp` must be compiled alongside `MyContainer.hpp` and linked against the `doctest` library.

### `main.cpp` - Demonstration File

This file provides a visual demonstration of how to use the container and each of its iterators. It prints the original container's contents and the results of each iterator's traversal to the console, allowing for verification of the correct element order. Furthermore, it showcases basic add and remove operations and exception handling.

### Building and Running with the Makefile:

Navigate to the project's root directory in your terminal and use the `make` command with the following targets:

* **Build All (Main Application and Tests)**:
    ```bash
    make all
    ```
    This command will create a `build` directory and compile both `main_app` and `my_container_tests` executables within it.

* **Build and Run the Main Application**:
    ```bash
    make Main
    ```
    This compiles `main.cpp` (if needed) and then executes the `main_app` program.

* **Build and Run the Unit Tests**:
    ```bash
    make test
    ```
    This compiles `Test.cpp` (if needed) and then executes the `my_container_tests` program, displaying the test results.

* **Run Valgrind on the Main Application**:
    ```bash
    make valgrind
    ```
    Performs a full memory leak check on the `main_app` executable.

* **Run Valgrind on the Unit Tests**:
    ```bash
    make valgrind test
    ```
    Performs a full memory leak check on the `my_container_tests` executable.

* **Clean Build Artifacts**:
    ```bash
    make clean
    ```
    Removes the `build` directory and all compiled executables.

* **View All Makefile Commands**:
    ```bash
    make help
    ```
    Displays a list of available `make` commands and their descriptions.

## Important Notes

* **Error Handling**: Iterators throw `std::out_of_range` when attempting to dereference an iterator pointing to an invalid position (such as `end()` or past it).
* **Snapshot Logic**: Iterators like `AscendingOrderIterator`, `DescendingOrderIterator`, `SideCrossOrderIterator`, and `MiddleOutOrderIterator` build a "snapshot" of the element/index order at their creation time. This means that modifications to the container (adding/removing elements) *after* an existing iterator has been created will not affect the traversal order of that specific iterator, but will affect any new iterators created subsequently.
* **Memory Management**: The container and its iterators utilize `std::vector` for element storage, benefiting from automatic memory management.
