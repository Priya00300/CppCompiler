# Makefile for C++ Compiler with Parser
# Compatible with your existing scanner and token structure

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0 -pedantic
TARGET = compiler
SRCDIR = .
OBJDIR = obj

# Source files - adjust these based on your actual file names
SOURCES = main.cpp tokens.cpp scanner.cpp parser.cpp
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)
HEADERS = tokens.hpp scanner.hpp parser.hpp

# Create obj directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Default target
all: $(OBJDIR) $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)
	@echo "✓ Compilation successful! Executable: $(TARGET)"

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(TARGET) test_*.cpp test_*.txt
	@echo "✓ Cleaned build artifacts"

# Create test files
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
	@echo "✓ Test files created: test_simple.cpp, test_control.cpp, test_loop.cpp, test_complex.cpp, test_types.cpp"

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

# Run all tests
test-all: test-simple test-control test-loop test-complex test-types
	@echo "✓ All tests completed!"

# Debug build
debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET)

# Release build
release: CXXFLAGS = -std=c++17 -O3 -DNDEBUG
release: $(TARGET)

# Check for common issues
check:
	@echo "Checking for required files..."
	@if [ ! -f "tokens.hpp" ]; then echo "❌ Missing tokens.hpp"; else echo "✓ tokens.hpp found"; fi
	@if [ ! -f "scanner.hpp" ]; then echo "❌ Missing scanner.hpp"; else echo "✓ scanner.hpp found"; fi
	@if [ ! -f "tokens.cpp" ]; then echo "❌ Missing tokens.cpp"; else echo "✓ tokens.cpp found"; fi
	@if [ ! -f "scanner.cpp" ]; then echo "❌ Missing scanner.cpp"; else echo "✓ scanner.cpp found"; fi

# Install (copy to /usr/local/bin)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/
	@echo "✓ Installed $(TARGET) to /usr/local/bin/"

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "✓ Uninstalled $(TARGET)"

# Help
help:
	@echo "Available targets:"
	@echo "  all           - Build the compiler (default)"
	@echo "  clean         - Remove build artifacts"
	@echo "  create-tests  - Create test input files"
	@echo "  test-simple   - Test simple arithmetic parsing"
	@echo "  test-control  - Test control structure parsing"
	@echo "  test-loop     - Test loop parsing"
	@echo "  test-complex  - Test complex expression parsing"
	@echo "  test-types    - Test different data types"
	@echo "  test-all      - Run all tests"
	@echo "  debug         - Build with debug symbols"
	@echo "  release       - Build optimized release version"
	@echo "  check         - Check for required source files"
	@echo "  install       - Install compiler to system"
	@echo "  uninstall     - Remove compiler from system"
	@echo "  help          - Show this help message"

# Dependencies
$(OBJDIR)/main.o: main.cpp $(HEADERS)
$(OBJDIR)/tokens.o: tokens.cpp tokens.hpp
$(OBJDIR)/scanner.o: scanner.cpp scanner.hpp tokens.hpp
$(OBJDIR)/parser.o: parser.cpp parser.hpp scanner.hpp tokens.hpp

.PHONY: all clean create-tests test-simple test-control test-loop test-complex test-types test-all debug release check install uninstall help
