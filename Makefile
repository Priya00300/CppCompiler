# C++ Compiler Project Makefile
# Builds a C++ compiler that generates x86-64 assembly

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
LDFLAGS =

# Target executable name
TARGET = compiler

# Source files (add all your .cpp files here)
SOURCES = main.cpp tokens.cpp scanner.cpp parser.cpp codegen.cpp
HEADERS = tokens.hpp scanner.hpp parser.hpp codegen.hpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

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

# Interactive expression compiler mode
interactive: $(TARGET)
	@echo "=== Interactive Expression Compiler ==="
	@echo "Enter expressions to compile (Ctrl+C to exit)"
	@echo "Examples: 2+3, 5*4-1, (2+3)*4, etc."
	@echo ""
	@while true; do \
		printf "Expression: "; \
		read expr || break; \
		if [ -z "$$expr" ]; then \
			echo "Empty expression, try again..."; \
			continue; \
		fi; \
		echo "$$expr;" > interactive_temp.cpp; \
		echo "Compiling: $$expr"; \
		if ./$(TARGET) interactive_temp.cpp -o interactive_temp.s 2>/dev/null; then \
			if as -64 interactive_temp.s -o interactive_temp.o 2>/dev/null; then \
				if ld interactive_temp.o -o interactive_temp_exe 2>/dev/null; then \
					./interactive_temp_exe 2>/dev/null; \
					result=$$?; \
					echo "✅ Result: $$result"; \
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

# Debug version of interactive mode
interactive-debug: $(TARGET)
	@echo "=== Interactive Expression Compiler (Debug Mode) ==="
	@echo "Enter expressions to compile (Ctrl+C to exit)"
	@echo ""
	@while true; do \
		printf "Expression: "; \
		read expr || break; \
		if [ -z "$$expr" ]; then \
			continue; \
		fi; \
		echo "$$expr;" > interactive_temp.cpp; \
		echo "--- Compiling: $$expr ---"; \
		echo "1. Compilation:"; \
		./$(TARGET) interactive_temp.cpp -o interactive_temp.s; \
		echo "2. Assembly:"; \
		as -64 interactive_temp.s -o interactive_temp.o; \
		echo "3. Linking:"; \
		ld interactive_temp.o -o interactive_temp_exe; \
		echo "4. Execution:"; \
		./interactive_temp_exe; echo "Exit code: $$?"; \
		echo "5. Generated Assembly:"; \
		cat interactive_temp.s; \
		rm -f interactive_temp.cpp interactive_temp.s interactive_temp.o interactive_temp_exe; \
		echo ""; \
	done

# Test specific expressions
test-specific: $(TARGET)
	@echo "=== Testing Specific Expressions ==="
	@echo ""

	@echo "Testing: 2+8 (expected: 10)"
	@echo '2+8;' > test_specific.cpp
	@./$(TARGET) test_specific.cpp -o test_specific.s
	@as -64 test_specific.s -o test_specific.o
	@ld test_specific.o -o test_specific_exe
	@./test_specific_exe; echo "✅ Actual result: $$?"
	@echo ""

	@echo "Testing: 1+1 (expected: 2)"
	@echo '1+1;' > test_specific.cpp
	@./$(TARGET) test_specific.cpp -o test_specific.s
	@as -64 test_specific.s -o test_specific.o
	@ld test_specific.o -o test_specific_exe
	@./test_specific_exe; echo "✅ Actual result: $$?"
	@echo ""

	@echo "Testing: 5 (expected: 5)"
	@echo '5;' > test_specific.cpp
	@./$(TARGET) test_specific.cpp -o test_specific.s
	@as -64 test_specific.s -o test_specific.o
	@ld test_specific.o -o test_specific_exe
	@./test_specific_exe; echo "✅ Actual result: $$?"
	@echo ""

	@echo "Testing: 10*2+3 (expected: 23)"
	@echo '10*2+3;' > test_specific.cpp
	@./$(TARGET) test_specific.cpp -o test_specific.s
	@as -64 test_specific.s -o test_specific.o
	@ld test_specific.o -o test_specific_exe
	@./test_specific_exe; echo "✅ Actual result: $$?"
	@echo ""

	@rm -f test_specific.cpp test_specific.s test_specific.o test_specific_exe
	@echo "✅ Specific expression tests completed"

# Test arithmetic operations
test-arithmetic: $(TARGET)
	@echo "=== Testing Arithmetic Operations ==="
	@echo ""

	@echo "Addition Tests:"
	@for expr in "1+1" "5+3" "10+20"; do \
		echo "  Testing: $$expr"; \
		echo "$$expr;" > test_arith.cpp; \
		./$(TARGET) test_arith.cpp -o test_arith.s; \
		as -64 test_arith.s -o test_arith.o; \
		ld test_arith.o -o test_arith_exe; \
		./test_arith_exe; echo "    Result: $$?"; \
	done
	@echo ""

	@echo "Multiplication Tests:"
	@for expr in "2*3" "5*4" "7*8"; do \
		echo "  Testing: $$expr"; \
		echo "$$expr;" > test_arith.cpp; \
		./$(TARGET) test_arith.cpp -o test_arith.s; \
		as -64 test_arith.s -o test_arith.o; \
		ld test_arith.o -o test_arith_exe; \
		./test_arith_exe; echo "    Result: $$?"; \
	done
	@echo ""

	@echo "Complex Expression Tests:"
	@for expr in "2+3*4" "(2+3)*4" "10-2*3"; do \
		echo "  Testing: $$expr"; \
		echo "$$expr;" > test_arith.cpp; \
		./$(TARGET) test_arith.cpp -o test_arith.s; \
		as -64 test_arith.s -o test_arith.o; \
		ld test_arith.o -o test_arith_exe; \
		./test_arith_exe; echo "    Result: $$?"; \
	done

	@rm -f test_arith.cpp test_arith.s test_arith.o test_arith_exe
	@echo "✅ Arithmetic tests completed"

# Quick test with a simple expression
test-simple: $(TARGET)
	@echo "=== Quick Simple Test ==="
	@echo "Testing: 2+3"
	@echo '2+3;' > quick_test.cpp
	@./$(TARGET) quick_test.cpp -o quick_test.s
	@as -64 quick_test.s -o quick_test.o
	@ld quick_test.o -o quick_test_exe
	@./quick_test_exe; echo "Result: $$? (should be 5)"
	@rm -f quick_test.cpp quick_test.s quick_test.o quick_test_exe
	@echo "✅ Quick test completed"

# Show project status
status:
	@echo "=== Project Status ==="
	@echo "Target executable: $(TARGET)"
	@echo -n "Compiler built: "; [ -f $(TARGET) ] && echo "✅ Yes" || echo "❌ No"
	@echo "Source files:"
	@for src in $(SOURCES); do \
		echo -n "  $$src: "; [ -f $$src ] && echo "✅ Found" || echo "❌ Missing"; \
	done
	@echo "Header files:"
	@for hdr in $(HEADERS); do \
		echo -n "  $$hdr: "; [ -f $$hdr ] && echo "✅ Found" || echo "❌ Missing"; \
	done
	@echo ""

# Install (copy to a system directory - optional)
install: $(TARGET)
	@echo "Installing $(TARGET) to /usr/local/bin/"
	@sudo cp $(TARGET) /usr/local/bin/
	@echo "✅ Installation completed"

# Help target
help:
	@echo "=== C++ Compiler Makefile Help ==="
	@echo ""
	@echo "Build targets:"
	@echo "  all                Build the compiler"
	@echo "  clean              Remove all build artifacts"
	@echo "  rebuild            Clean and build"
	@echo ""
	@echo "Testing targets:"
	@echo "  interactive        Start interactive expression compiler"
	@echo "  interactive-debug  Interactive mode with debug output"
	@echo "  test-simple        Quick test with 2+3"
	@echo "  test-specific      Test specific expressions"
	@echo "  test-arithmetic    Test various arithmetic operations"
	@echo ""
	@echo "Utility targets:"
	@echo "  status             Show project status"
	@echo "  help               Show this help"
	@echo "  install            Install to system (requires sudo)"
	@echo ""
	@echo "Usage example:"
	@echo "  make clean && make all && make test-simple"

# Declare phony targets
.PHONY: all clean rebuild interactive interactive-debug test-specific test-arithmetic test-simple status help install

# Default target when just running 'make'
.DEFAULT_GOAL := all
