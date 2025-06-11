# General settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS =

# Directories
BUILD_DIR = ./build

# Source and header files
MY_CONTAINER_H = MyContainer.hpp # Corrected based on your ls output
MAIN_SRC = Main.cpp
TEST_SRC = Test.cpp              # Corrected based on your ls output
DOCTEST_H = doctest.h

# Executables
MAIN_TARGET = $(BUILD_DIR)/main_app
TEST_TARGET = $(BUILD_DIR)/my_container_tests

# Phony targets
.PHONY: all Main test valgrind valgrind_test clean

all: Main test

# --- Main Application Target ---
Main: $(MAIN_TARGET)
	@echo "Running Main application..."
	@$(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_SRC) $(MY_CONTAINER_H)
	@mkdir -p $(BUILD_DIR) # Ensure build directory exists
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

# --- Unit Tests Target ---
test: $(TEST_TARGET)
	@echo "Running unit tests..."
	@$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) $(MY_CONTAINER_H) $(DOCTEST_H)
	@mkdir -p $(BUILD_DIR) # Ensure build directory exists
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

# --- Valgrind Targets ---
valgrind: $(MAIN_TARGET)
	@echo "Running Valgrind memory leak check on Main application..."
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(MAIN_TARGET)

valgrind_test: $(TEST_TARGET)
	@echo "Running Valgrind memory leak check on Unit Tests..."
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(TEST_TARGET)

# --- Clean Target ---
clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR)

# --- Help Target ---
help:
	@echo "Makefile commands:"
	@echo "  make all          - Build both Main application and unit tests"
	@echo "  make Main        - Build and run the Main application"
	@echo "  make test        - Build and run the unit tests"
	@echo "  make valgrind    - Run Valgrind on the Main application"
	@echo "  make valgrind test - Run Valgrind on the unit tests"
	@echo "  make clean       - Clean build artifacts"
	@echo "  make help        - Show this help message"
# --- Default target ---