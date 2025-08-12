# C++ Compiler Project Makefile
# Builds a C++ compiler that generates x86-64 assembly

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
LDFLAGS =

# Target executable name
TARGET = compiler

# Source files - FIXED: Ensure all cpp files exist
SOURCES = main.cpp tokens.cpp scanner.cpp parser.cpp codegen.cpp symboltable.cpp
HEADERS = tokens.hpp scanner.hpp parser.hpp codegen.hpp symboltable.hpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: check-files $(TARGET)

# Check that all source files exist
check-files:
	@echo "🔍 Checking for required source files..."
	@for file in $(SOURCES); do \
		if [ ! -f "$$file" ]; then \
			echo "❌ Missing source file: $$file"; \
			echo "   Please create this file or remove it from SOURCES in Makefile"; \
			exit 1; \
		fi; \
	done
	@for file in $(HEADERS); do \
		if [ ! -f "$$file" ]; then \
			echo "❌ Missing header file: $$file"; \
			echo "   Please create this file or remove it from HEADERS in Makefile"; \
			exit 1; \
		fi; \
	done
	@echo "✅ All required files found"

# Build the main compiler executable
$(TARGET): $(OBJECTS)
	@echo "🔗 Linking $(TARGET)..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✅ Build completed: $(TARGET)"

# Compile source files to object files
%.o: %.cpp $(HEADERS)
	@echo "🔨 Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f $(OBJECTS) $(TARGET)
	rm -f *.s *.o *_exe test_*.cpp interactive_temp*
	rm -f simple_test.cpp simple_test.s test_specific.*
	@echo "✅ Clean completed"

# Force rebuild
rebuild: clean all

# Create missing files with templates
create-missing:
	@echo "📝 Creating missing source file templates..."
	@if [ ! -f "symboltable.cpp" ]; then \
		echo "Creating symboltable.cpp..."; \
		cat > symboltable.cpp << 'EOF'; \
#include "symboltable.hpp"; \
#include <iostream>; \
; \
bool SymbolTable::addSymbol(const std::string& name, SymbolType type) {; \
    if (exists(name)) {; \
        return false;; \
    }; \
    symbols.emplace(name, Symbol(name, type, currentOffset, currentScope));; \
    currentOffset -= 8;; \
    return true;; \
}; \
; \
Symbol* SymbolTable::findSymbol(const std::string& name) {; \
    auto it = symbols.find(name);; \
    return (it != symbols.end()) ? &it->second : nullptr;; \
}; \
; \
bool SymbolTable::exists(const std::string& name) const {; \
    return symbols.find(name) != symbols.end();; \
}; \
; \
void SymbolTable::markInitialized(const std::string& name) {; \
    auto it = symbols.find(name);; \
    if (it != symbols.end()) {; \
        it->second.initialized = true;; \
    }; \
}; \
; \
const std::unordered_map<std::string, Symbol>& SymbolTable::getAllSymbols() const {; \
    return symbols;; \
}; \
; \
void SymbolTable::clear() {; \
    symbols.clear();; \
    currentOffset = -8;; \
    currentScope = 0;; \
}; \
; \
void SymbolTable::enterScope() {; \
    currentScope++;; \
}; \
; \
void SymbolTable::exitScope() {; \
    auto it = symbols.begin();; \
    while (it != symbols.end()) {; \
        if (it->second.scope >= currentScope) {; \
            it = symbols.erase(it);; \
        } else {; \
            ++it;; \
        }; \
    }; \
    if (currentScope > 0) {; \
        currentScope--;; \
    }; \
}; \
EOF; \
	fi
	@echo "✅ Missing files created"

# Interactive expression compiler mode
interactive: $(TARGET)
	@echo "=== Interactive Expression Compiler ==="
	@echo "Enter expressions to compile (Ctrl+C to exit)"
	@echo "Examples: 2+3, 5*4-1, (2+3)*4, etc."
	@echo ""
	@while true; do \
		printf "Expression: "; \
		read expr || break; \
		if [ -z "$expr" ]; then \
			echo "Empty expression, try again..."; \
			continue; \
		fi; \
		echo "$expr;" > interactive_temp.cpp; \
		echo "Compiling: $expr"; \
		if ./$(TARGET) interactive_temp.cpp -o interactive_temp.s 2>/dev/null; then \
			if as -64 interactive_temp.s -o interactive_temp.o 2>/dev/null; then \
				if ld interactive_temp.o -o interactive_temp_exe 2>/dev/null; then \
					./interactive_temp_exe 2>/dev/null; \
					result=$?; \
					echo "✅ Result: $result"; \
				else \
					echo "❌ Linking failed"; \
				fi; \
			else \
				echo "❌ Assembly failed"; \
			fi; \
		else \
			echo "❌ Compilation failed"; \
		fi; \
		rm -f interactive_temp.cpp interactive_temp.s interactive_temp.o interactive_temp_exe 2>/dev/null; \
		echo ""; \
	done
	@echo "Interactive session ended."

# Test statement compilation
test-statements: $(TARGET)
	@echo "=== Testing Statement Compilation ==="
	@echo ""

	@echo "Testing variable declaration:"
	@echo 'int x = 5; x;' > test_stmt.cpp
	@./$(TARGET) test_stmt.cpp -o test_stmt.s
	@as -64 test_stmt.s -o test_stmt.o
	@ld test_stmt.o -o test_stmt_exe
	@./test_stmt_exe; echo "✅ Result: $? (should be 5)"
	@echo ""

	@echo "Testing multiple statements:"
	@echo 'int a = 2; int b = 3; a + b;' > test_mult.cpp
	@./$(TARGET) test_mult.cpp -o test_mult.s
	@as -64 test_mult.s -o test_mult.o
	@ld test_mult.o -o test_mult_exe
	@./test_mult_exe; echo "✅ Result: $? (should be 5)"
	@echo ""

	@rm -f test_stmt.cpp test_stmt.s test_stmt.o test_stmt_exe
	@rm -f test_mult.cpp test_mult.s test_mult.o test_mult_exe
	@echo "✅ Statement tests completed"

# Quick test with a simple expression
test-simple: $(TARGET)
	@echo "=== Quick Simple Test ==="
	@echo "Testing: 2+3"
	@echo '2+3;' > quick_test.cpp
	@./$(TARGET) quick_test.cpp -o quick_test.s
	@as -64 quick_test.s -o quick_test.o
	@ld quick_test.o -o quick_test_exe
	@./quick_test_exe; echo "Result: $? (should be 5)"
	@rm -f quick_test.cpp quick_test.s quick_test.o quick_test_exe
	@echo "✅ Quick test completed"

# Show project status
status:
	@echo "=== Project Status ==="
	@echo "Target executable: $(TARGET)"
	@echo -n "Compiler built: "; [ -f $(TARGET) ] && echo "✅ Yes" || echo "❌ No"
	@echo "Source files:"
	@for src in $(SOURCES); do \
		echo -n "  $src: "; [ -f $src ] && echo "✅ Found" || echo "❌ Missing"; \
	done
	@echo "Header files:"
	@for hdr in $(HEADERS); do \
		echo -n "  $hdr: "; [ -f $hdr ] && echo "✅ Found" || echo "❌ Missing"; \
	done
	@echo ""

# Help target
help:
	@echo "=== C++ Compiler Makefile Help ==="
	@echo ""
	@echo "Build targets:"
	@echo "  all                Build the compiler"
	@echo "  clean              Remove all build artifacts"
	@echo "  rebuild            Clean and build"
	@echo "  create-missing     Create template files for missing sources"
	@echo ""
	@echo "Testing targets:"
	@echo "  interactive        Start interactive expression compiler"
	@echo "  test-simple        Quick test with 2+3"
	@echo "  test-statements    Test variable declarations and statements"
	@echo ""
	@echo "Utility targets:"
	@echo "  status             Show project status"
	@echo "  help               Show this help"
	@echo ""
	@echo "Usage example:"
	@echo "  make clean && make all && make test-simple"

# Declare phony targets
.PHONY: all clean rebuild create-missing interactive test-statements test-simple status help check-files

# Default target when just running 'make'
.DEFAULT_GOAL := all
