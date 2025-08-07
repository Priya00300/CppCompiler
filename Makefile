# Makefile for C++ Compiler with Code Generation
# Now includes AST → x86-64 Assembly compilation

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0 -pedantic
TARGET = compiler
EXPR_TEST = expr_test
SRCDIR = .
OBJDIR = obj

# Source files - now includes codegen
MAIN_SOURCES = main.cpp tokens.cpp scanner.cpp parser.cpp codegen.cpp
EXPR_SOURCES = expr_test.cpp tokens.cpp scanner.cpp parser.cpp
MAIN_OBJECTS = $(MAIN_SOURCES:%.cpp=$(OBJDIR)/%.o)
EXPR_OBJECTS = $(EXPR_SOURCES:%.cpp=$(OBJDIR)/%.o)
HEADERS = tokens.hpp scanner.hpp parser.hpp codegen.hpp

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
	rm -rf $(OBJDIR) $(TARGET) $(EXPR_TEST) test_*.cpp test_*.s test_*.o test_* *.s *.o executable_*
	@echo "✓ Cleaned build artifacts"

# === CODE GENERATION TESTS ===

# Create test files for code generation
create-codegen-tests:
	@echo "Creating code generation test files..."
	@echo '2 + 3 * 4;' > test_arithmetic.cpp
	@echo 'Arithmetic test created: 2 + 3 * 4 (should exit with code 14)'
	@echo ""
	@echo '10 - 2 * 3;' > test_subtract.cpp
	@echo 'Subtraction test created: 10 - 2 * 3 (should exit with code 4)'
	@echo ""
	@echo '15 / 3 + 2;' > test_division.cpp
	@echo 'Division test created: 15 / 3 + 2 (should exit with code 7)'
	@echo ""
	@echo '-5 + 8;' > test_unary.cpp
	@echo 'Unary test created: -5 + 8 (should exit with code 3)'
	@echo ""
	@echo '(2 + 3) * (4 - 1);' > test_parentheses.cpp
	@echo 'Parentheses test created: (2 + 3) * (4 - 1) (should exit with code 15)'
	@echo ""
	@echo '2 == 2;' > test_comparison.cpp
	@echo 'Comparison test created: 2 == 2 (should exit with code 1 for true)'
	@echo ""
	@echo '5 > 3 && 2 < 4;' > test_logical.cpp
	@echo 'Logical test created: 5 > 3 && 2 < 4 (should exit with code 1 for true)'
	@echo "✓ Code generation test files created"

# Test basic arithmetic code generation
test-arithmetic: $(TARGET)
	@echo "=== Testing Arithmetic Code Generation ==="
	@echo "Expression: 2 + 3 * 4 (expected result: 14)"
	./$(TARGET) test_arithmetic.cpp
	as -64 test_arithmetic.s -o test_arithmetic.o
	ld test_arithmetic.o -o test_arithmetic_exe
	./test_arithmetic_exe; echo "Exit code: $$?"
	@echo "✓ Arithmetic test completed"

# Test subtraction
test-subtract: $(TARGET)
	@echo "=== Testing Subtraction ==="
	@echo "Expression: 10 - 2 * 3 (expected result: 4)"
	./$(TARGET) test_subtract.cpp
	as -64 test_subtract.s -o test_subtract.o
	ld test_subtract.o -o test_subtract_exe
	./test_subtract_exe; echo "Exit code: $$?"
	@echo "✓ Subtraction test completed"

# Test division
test-division: $(TARGET)
	@echo "=== Testing Division ==="
	@echo "Expression: 15 / 3 + 2 (expected result: 7)"
	./$(TARGET) test_division.cpp
	as -64 test_division.s -o test_division.o
	ld test_division.o -o test_division_exe
	./test_division_exe; echo "Exit code: $$?"
	@echo "✓ Division test completed"

# Test unary operators
test-unary: $(TARGET)
	@echo "=== Testing Unary Operators ==="
	@echo "Expression: -5 + 8 (expected result: 3)"
	./$(TARGET) test_unary.cpp
	as -64 test_unary.s -o test_unary.o
	ld test_unary.o -o test_unary_exe
	./test_unary_exe; echo "Exit code: $$?"
	@echo "✓ Unary test completed"

# Test parentheses
test-parentheses: $(TARGET)
	@echo "=== Testing Parentheses ==="
	@echo "Expression: (2 + 3) * (4 - 1) (expected result: 15)"
	./$(TARGET) test_parentheses.cpp
	as -64 test_parentheses.s -o test_parentheses.o
	ld test_parentheses.o -o test_parentheses_exe
	./test_parentheses_exe; echo "Exit code: $$?"
	@echo "✓ Parentheses test completed"

# Test comparison operators
test-comparison: $(TARGET)
	@echo "=== Testing Comparison ==="
	@echo "Expression: 2 == 2 (expected result: 1 for true)"
	./$(TARGET) test_comparison.cpp
	as -64 test_comparison.s -o test_comparison.o
	ld test_comparison.o -o test_comparison_exe
	./test_comparison_exe; echo "Exit code: $$?"
	@echo "✓ Comparison test completed"

# Test logical operators
test-logical: $(TARGET)
	@echo "=== Testing Logical Operators ==="
	@echo "Expression: 5 > 3 && 2 < 4 (expected result: 1 for true)"
	./$(TARGET) test_logical.cpp
	as -64 test_logical.s -o test_logical.o
	ld test_logical.o -o test_logical_exe
	./test_logical_exe; echo "Exit code: $$?"
	@echo "✓ Logical test completed"

# Test code generation step by step
test-codegen-verbose: $(TARGET)
	@echo "=== Verbose Code Generation Test ==="
	@echo "Compiling: 2 + 3 * 4"
	./$(TARGET) --verbose test_arithmetic.cpp
	@echo ""
	@echo "Generated Assembly:"
	@echo "==================="
	cat test_arithmetic.s
	@echo "==================="
	@echo ""
	@echo "Assembling and running..."
	as -64 test_arithmetic.s -o test_arithmetic.o
	ld test_arithmetic.o -o test_arithmetic_exe
	./test_arithmetic_exe; echo "Final exit code: $$?"

# Test assembly output to stdout
test-stdout: $(TARGET)
	@echo "=== Testing Assembly Output to Stdout ==="
	./$(TARGET) --to-stdout test_arithmetic.cpp

# Run all code generation tests
test-all-codegen: create-codegen-tests test-arithmetic test-subtract test-division test-unary test-parentheses test-comparison test-logical
	@echo "✓ All code generation tests completed!"

# === PRECEDENCE TESTS (from previous version) ===

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

# === DEVELOPMENT WORKFLOW ===

# Complete development workflow test
test-workflow: $(TARGET)
	@echo "=== Complete Development Workflow ==="
	@echo "1. Creating test expression..."
	@echo '7 * 6 - 2;' > workflow_test.cpp

	@echo "2. Compiling to assembly..."
	./$(TARGET) workflow_test.cpp -o workflow_test.s

	@echo "3. Generated assembly:"
	@echo "---------------------"
	cat workflow_test.s
	@echo "---------------------"

	@echo "4. Assembling..."
	as -64 workflow_test.s -o workflow_test.o

	@echo "5. Linking..."
	ld workflow_test.o -o workflow_test_exe

	@echo "6. Running and checking result..."
	./workflow_test_exe; echo "Result: $$? (expected: 40)"

	@echo "7. Cleanup..."
	rm -f workflow_test.cpp workflow_test.s workflow_test.o workflow_test_exe

	@echo "✓ Complete workflow test finished!"

# === UTILITY TARGETS ===

# Quick test - compile and run a simple expression
quick-test: $(TARGET)
	@echo "=== Quick Test ==="
	@echo '5 + 3 * 2;' > quick.cpp
	./$(TARGET) quick.cpp
	as -64 quick.s -o quick.o && ld quick.o -o quick_exe
	./quick_exe; echo "Result: $$? (expected: 11)"
	rm -f quick.cpp quick.s quick.o quick_exe

# Interactive expression compilation
interactive: $(TARGET)
	@echo "=== Interactive Expression Compiler ==="
	@echo "Enter expressions to compile (Ctrl+C to exit)"
	@while true; do \
		read -p "Expression: " expr; \
		echo "$$expr;" > interactive_temp.cpp; \
		./$(TARGET) interactive_temp.cpp -o interactive_temp.s; \
		as -64 interactive_temp.s -o interactive_temp.o; \
		ld interactive_temp.o -o interactive_temp_exe; \
		./interactive_temp_exe; echo "Result: $$?"; \
		rm -f interactive_temp.cpp interactive_temp.s interactive_temp.o interactive_temp_exe; \
	done

# Show generated assembly for an expression
show-asm: $(TARGET)
	@read -p "Enter expression: " expr; \
	echo "$$expr;" > show_temp.cpp; \
	echo "Generated assembly for: $$expr"; \
	echo "==============================="; \
	./$(TARGET) --to-stdout show_temp.cpp; \
	rm -f show_temp.cpp

# === BUILD VARIANTS ===

# Debug build
debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET) $(EXPR_TEST)

# Release build
release: CXXFLAGS = -std=c++17 -O3 -DNDEBUG
release: $(TARGET) $(EXPR_TEST)

# === LEGACY TESTS ===

# Create standard test files (from previous version)
create-tests:
	@echo "Creating standard test files..."
	@echo 'int x = 5 + 3 * 2;' > test_simple.cpp
	@echo 'cout << x;' >> test_simple.cpp
	@echo "✓ Standard test files created"

# === HELP AND INFO ===

# Help
help:
	@echo "C++ Compiler Makefile - Code Generation Phase"
	@echo "=============================================="
	@echo ""
	@echo "Build Targets:"
	@echo "  all                     - Build compiler and expression tester (default)"
	@echo "  clean                   - Remove all build artifacts"
	@echo "  debug                   - Build with debug symbols"
	@echo "  release                 - Build optimized release version"
	@echo ""
	@echo "Code Generation Tests:"
	@echo "  create-codegen-tests    - Create test files for code generation"
	@echo "  test-arithmetic         - Test arithmetic operations"
	@echo "  test-subtract           - Test subtraction"
	@echo "  test-division           - Test division"
	@echo "  test-unary              - Test unary operators"
	@echo "  test-parentheses        - Test parentheses precedence"
	@echo "  test-comparison         - Test comparison operators"
	@echo "  test-logical            - Test logical operators"
	@echo "  test-all-codegen        - Run all code generation tests"
	@echo "  test-codegen-verbose    - Verbose code generation test"
	@echo ""
	@echo "Development Workflow:"
	@echo "  test-workflow           - Complete compile→assemble→link→run cycle"
	@echo "  quick-test              - Quick expression compilation test"
	@echo "  interactive             - Interactive expression compiler"
	@echo "  show-asm                - Show assembly for an expression"
	@echo "  test-stdout             - Test assembly output to stdout"
	@echo ""
	@echo "Precedence Tests:"
	@echo "  test-precedence         - Test operator precedence parsing"
	@echo ""
	@echo "Manual Usage:"
	@echo "  ./compiler file.cpp              → file.s"
	@echo "  ./compiler file.cpp -o out.s     → out.s"
	@echo "  ./compiler --to-stdout file.cpp  → stdout"
	@echo "  ./compiler --verbose file.cpp    → detailed output"
	@echo "  as -64 file.s -o file.o          → assemble"
	@echo "  ld file.o -o executable          → link"
	@echo "  ./executable; echo $$?            → run and show result"

# Check for required files
check:
	@echo "Checking for required files..."
	@for file in tokens.hpp scanner.hpp parser.hpp codegen.hpp tokens.cpp scanner.cpp parser.cpp codegen.cpp main.cpp expr_test.cpp; do \
		if [ -f "$$file" ]; then echo "✓ $$file found"; else echo "❌ Missing $$file"; fi; \
	done

# Dependencies
$(OBJDIR)/main.o: main.cpp $(HEADERS)
$(OBJDIR)/tokens.o: tokens.cpp tokens.hpp
$(OBJDIR)/scanner.o: scanner.cpp scanner.hpp tokens.hpp
$(OBJDIR)/parser.o: parser.cpp parser.hpp scanner.hpp tokens.hpp
$(OBJDIR)/codegen.o: codegen.cpp codegen.hpp parser.hpp
$(OBJDIR)/expr_test.o: expr_test.cpp parser.hpp scanner.hpp tokens.hpp

.PHONY: all clean create-codegen-tests test-arithmetic test-subtract test-division test-unary test-parentheses test-comparison test-logical test-all-codegen test-codegen-verbose test-stdout test-precedence test-workflow quick-test interactive show-asm create-tests debug release help check
