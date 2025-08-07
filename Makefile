# Makefile for C++ Compiler with Precedence Parser
# Compatible with your existing scanner and token structure

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0 -pedantic
TARGET = compiler
EXPR_TEST = expr_test
SRCDIR = .
OBJDIR = obj

# Source files - adjust these based on your actual file names
MAIN_SOURCES = main.cpp tokens.cpp scanner.cpp parser.cpp
EXPR_SOURCES = expr_test.cpp tokens.cpp scanner.cpp parser.cpp
MAIN_OBJECTS = $(MAIN_SOURCES:%.cpp=$(OBJDIR)/%.o)
EXPR_OBJECTS = $(EXPR_SOURCES:%.cpp=$(OBJDIR)/%.o)
HEADERS = tokens.hpp scanner.hpp parser.hpp

# Create obj directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Default target
all: $(OBJDIR) $(TARGET) $(EXPR_TEST)

# Link object files to create main compiler executable
$(TARGET): $(MAIN_OBJECTS)
	$(CXX) $(MAIN_OBJECTS) -o $(TARGET)
	@echo "✓ Main compiler built successfully: $(TARGET)"

# Link object files to create expression tester
$(EXPR_TEST): $(EXPR_OBJECTS)
	$(CXX) $(EXPR_OBJECTS) -o $(EXPR_TEST)
	@echo "✓ Expression tester built successfully: $(EXPR_TEST)"

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(TARGET) $(EXPR_TEST) test_*.cpp test_*.txt precedence_test_*.txt
	@echo "✓ Cleaned build artifacts"

# Create test files for precedence testing
create-precedence-tests:
	@echo "Creating precedence test files..."
	@echo '2 + 3 * 4;' > precedence_test_1.txt
	@echo '2 * 3 + 4 * 5;' >> precedence_test_1.txt
	@echo ""
	@echo '(2 + 3) * 4;' > precedence_test_2.txt
	@echo '2 * (3 + 4);' >> precedence_test_2.txt
	@echo ""
	@echo '-2 + 3 * 4;' > precedence_test_3.txt
	@echo '2 + -3 * 4;' >> precedence_test_3.txt
	@echo ""
	@echo 'a = b + c * d;' > precedence_test_4.txt
	@echo 'x + y == z * w;' >> precedence_test_4.txt
	@echo ""
	@echo 'a && b || c;' > precedence_test_5.txt
	@echo 'a || b && c;' >> precedence_test_5.txt
	@echo "✓ Precedence test files created"

# Test operator precedence specifically
test-precedence: $(EXPR_TEST)
	@echo "=== Testing Operator Precedence ==="
	@echo ""
	@echo "Testing: 2 + 3 * 4 (should be 2 + (3 * 4))"
	./$(EXPR_TEST) "2 + 3 * 4"
	@echo ""
	@echo "Testing: 2 * 3 + 4 * 5 (should be (2 * 3) + (4 * 5))"
	./$(EXPR_TEST) "2 * 3 + 4 * 5"
	@echo ""
	@echo "Testing: (2 + 3) * 4 (should respect parentheses)"
	./$(EXPR_TEST) "(2 + 3) * 4"
	@echo ""
	@echo "Testing: -2 + 3 (should be (-2) + 3)"
	./$(EXPR_TEST) "-2 + 3"
	@echo ""
	@echo "Testing: a && b || c (should be (a && b) || c)"
	./$(EXPR_TEST) "a && b || c"

# Test precedence interactively
test-precedence-interactive: $(EXPR_TEST)
	@echo "=== Interactive Precedence Testing ==="
	./$(EXPR_TEST)

# Create standard test files
create-tests:
	@echo "Creating test files..."
	@echo 'int x = 5 + 3 * 2;' > test_simple.cpp
	@echo 'cout << x;' >> test_simple.cpp
	@echo ""
	@echo 'int x = 10;' > test_control.cpp
	@echo 'if (x > 5) {' >> test_control.cpp
	@echo '    cout << "x is greater than 5";' >> test_control.cpp
	@echo '} else {' >> test_control.cpp
	@echo '    cout << "x is not greater than 5";' >> test_control.cpp
	@echo '}' >> test_control.cpp
	@echo ""
	@echo 'int i = 0;' > test_loop.cpp
	@echo 'while (i < 10) {' >> test_loop.cpp
	@echo '    cout << i;' >> test_loop.cpp
	@echo '    i = i + 1;' >> test_loop.cpp
	@echo '}' >> test_loop.cpp
	@echo ""
	@echo 'int a = 5;' > test_complex.cpp
	@echo 'int b = 10;' >> test_complex.cpp
	@echo 'int result = (a + b) * 2 - 3;' >> test_complex.cpp
	@echo 'cout << "Result: " << result;' >> test_complex.cpp
	@echo 'if (result > 20) {' >> test_complex.cpp
	@echo '    cout << "Result is large";' >> test_complex.cpp
	@echo '}' >> test_complex.cpp
	@echo ""
	@echo 'float x = 3.14;' > test_types.cpp
	@echo 'char c = "a";' >> test_types.cpp
	@echo 'bool flag = true;' >> test_types.cpp
	@echo 'cout << x << c << flag;' >> test_types.cpp
	@echo ""
	@echo 'int result = 2 + 3 * 4 - 1;' > test_precedence.cpp
	@echo 'bool check = result == 13 && result > 10;' >> test_precedence.cpp
	@echo 'cout << "Result: " << result << " Check: " << check;' >> test_precedence.cpp
	@echo "✓ Test files created"

# Test with simple arithmetic
test-simple: $(TARGET) create-tests
	@echo "=== Testing Simple Arithmetic ==="
	./$(TARGET) test_simple.cpp

# Test with control structures
test-control: $(TARGET) create-tests
	@echo "=== Testing Control Structures ==="
	./$(TARGET) test_control.cpp

# Test with loops
test-loop: $(TARGET) create-tests
	@echo "=== Testing Loops ==="
	./$(TARGET) test_loop.cpp

# Test with complex expressions
test-complex: $(TARGET) create-tests
	@echo "=== Testing Complex Expressions ==="
	./$(TARGET) test_complex.cpp

# Test with different types
test-types: $(TARGET) create-tests
	@echo "=== Testing Different Types ==="
	./$(TARGET) test_types.cpp

# Test precedence with full programs
test-precedence-programs: $(TARGET) create-tests
	@echo "=== Testing Precedence in Programs ==="
	./$(TARGET) test_precedence.cpp

# Run all standard tests
test-all: test-simple test-control test-loop test-complex test-types test-precedence-programs
	@echo "✓ All program tests completed!"

# Run all precedence-specific tests
test-all-precedence: test-precedence test-precedence-programs
	@echo "✓ All precedence tests completed!"

# Debug build
debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET) $(EXPR_TEST)

# Release build
release: CXXFLAGS = -std=c++17 -O3 -DNDEBUG
release: $(TARGET) $(EXPR_TEST)

# Check for common issues
check:
	@echo "Checking for required files..."
	@if [ ! -f "tokens.hpp" ]; then echo "❌ Missing tokens.hpp"; else echo "✓ tokens.hpp found"; fi
	@if [ ! -f "scanner.hpp" ]; then echo "❌ Missing scanner.hpp"; else echo "✓ scanner.hpp found"; fi
	@if [ ! -f "parser.hpp" ]; then echo "❌ Missing parser.hpp"; else echo "✓ parser.hpp found"; fi
	@if [ ! -f "tokens.cpp" ]; then echo "❌ Missing tokens.cpp"; else echo "✓ tokens.cpp found"; fi
	@if [ ! -f "scanner.cpp" ]; then echo "❌ Missing scanner.cpp"; else echo "✓ scanner.cpp found"; fi
	@if [ ! -f "parser.cpp" ]; then echo "❌ Missing parser.cpp"; else echo "✓ parser.cpp found"; fi
	@if [ ! -f "expr_test.cpp" ]; then echo "❌ Missing expr_test.cpp"; else echo "✓ expr_test.cpp found"; fi

# Install (copy to /usr/local/bin)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/
	@echo "✓ Installed $(TARGET) to /usr/local/bin/"

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "✓ Uninstalled $(TARGET)"

# Compare precedence parsers (if you want to implement both Pratt and recursive descent)
compare-parsers: $(EXPR_TEST)
	@echo "=== Comparing Parser Results ==="
	@echo "Testing expression: 2 + 3 * 4 - 1"
	@echo "Expected result: 2 + (3 * 4) - 1 = 13"
	./$(EXPR_TEST) "2 + 3 * 4 - 1"
	@echo ""
	@echo "Testing expression: a = b + c * d"
	@echo "Expected result: a = (b + (c * d))"
	./$(EXPR_TEST) "a = b + c * d"

# Help
help:
	@echo "Available targets:"
	@echo "  all                    - Build both compiler and expression tester (default)"
	@echo "  clean                  - Remove build artifacts"
	@echo "  create-tests           - Create standard test input files"
	@echo "  create-precedence-tests- Create precedence-specific test files"
	@echo ""
	@echo "Standard Tests:"
	@echo "  test-simple            - Test simple arithmetic parsing"
	@echo "  test-control           - Test control structure parsing"
	@echo "  test-loop              - Test loop parsing"
	@echo "  test-complex           - Test complex expression parsing"
	@echo "  test-types             - Test different data types"
	@echo "  test-all               - Run all standard tests"
	@echo ""
	@echo "Precedence Tests:"
	@echo "  test-precedence        - Test operator precedence (expressions only)"
	@echo "  test-precedence-interactive - Interactive precedence testing"
	@echo "  test-precedence-programs - Test precedence in full programs"
	@echo "  test-all-precedence    - Run all precedence tests"
	@echo "  compare-parsers        - Compare different parser implementations"
	@echo ""
	@echo "Build Variants:"
	@echo "  debug                  - Build with debug symbols"
	@echo "  release                - Build optimized release version"
	@echo ""
	@echo "Utilities:"
	@echo "  check                  - Check for required source files"
	@echo "  install                - Install compiler to system"
	@echo "  uninstall              - Remove compiler from system"
	@echo "  help                   - Show this help message"

# Dependencies
$(OBJDIR)/main.o: main.cpp $(HEADERS)
$(OBJDIR)/tokens.o: tokens.cpp tokens.hpp
$(OBJDIR)/scanner.o: scanner.cpp scanner.hpp tokens.hpp
$(OBJDIR)/parser.o: parser.cpp parser.hpp scanner.hpp tokens.hpp
$(OBJDIR)/expr_test.o: expr_test.cpp parser.hpp scanner.hpp tokens.hpp

.PHONY: all clean create-tests create-precedence-tests test-simple test-control test-loop test-complex test-types test-precedence test-precedence-interactive test-precedence-programs test-all test-all-precedence debug release check install uninstall compare-parsers help
