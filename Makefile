# Makefile for C++ Lexical Scanner

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
TARGET = scanner
SOURCES = main.cpp scanner.cpp tokens.cpp
HEADERS = scanner.hpp tokens.hpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the scanner executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Test with sample file
test: $(TARGET)
	@echo "Creating test file..."
	@echo '#include <iostream>' > test.cpp
	@echo 'using namespace std;' >> test.cpp
	@echo '' >> test.cpp
	@echo 'int main() {' >> test.cpp
	@echo '    int x = 42;' >> test.cpp
	@echo '    float y = 3.14f;' >> test.cpp
	@echo '    cout << "Hello World!" << endl;' >> test.cpp
	@echo '    return 0;' >> test.cpp
	@echo '}' >> test.cpp
	@echo "Running scanner on test.cpp..."
	./$(TARGET) test.cpp

# Debug build
debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET)

# Release build
release: CXXFLAGS += -DNDEBUG -O3
release: clean $(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the scanner (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  test     - Build and test with sample file"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release version"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  help     - Show this help message"

.PHONY: all clean test debug release install uninstall help
