#!/bin/bash

# Comprehensive Test Suite for C++ Compiler
# Tests all features defined in parser.hpp

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run a single test
run_test() {
    local test_name="$1"
    local input_code="$2"
    local expected_result="$3"

    echo -e "${YELLOW}Testing: $test_name${NC}"
    echo "Code: $input_code"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    # Create test file
    echo "$input_code" > test_temp.cpp

    # Compile
    if ! ./compiler test_temp.cpp -o test_temp.s 2>/dev/null; then
        echo -e "${RED}❌ FAIL: Compilation failed${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo ""
        return 1
    fi

    # Assemble
    if ! as -64 test_temp.s -o test_temp.o 2>/dev/null; then
        echo -e "${RED}❌ FAIL: Assembly failed${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo ""
        return 1
    fi

    # Link
    if ! ld test_temp.o -o test_temp_exe 2>/dev/null; then
        echo -e "${RED}❌ FAIL: Linking failed${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo ""
        return 1
    fi

    # Execute
    ./test_temp_exe 2>/dev/null
    local actual_result=$?

    # Check result
    if [ "$actual_result" -eq "$expected_result" ]; then
        echo -e "${GREEN}✅ PASS: Got $actual_result (expected $expected_result)${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ FAIL: Got $actual_result (expected $expected_result)${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi

    echo ""
    rm -f test_temp.cpp test_temp.s test_temp.o test_temp_exe
}

echo "==============================================="
echo "    C++ Compiler Comprehensive Test Suite"
echo "==============================================="

# Check if compiler exists
if [ ! -f "./compiler" ]; then
    echo -e "${RED}Error: ./compiler not found. Please run 'make' first.${NC}"
    exit 1
fi

echo "Testing all language features from parser.hpp..."
echo ""

# ==============================================
# PHASE 1: BASIC EXPRESSIONS & LITERALS
# ==============================================
echo "=== PHASE 1: BASIC EXPRESSIONS & LITERALS ==="

# Integer literals
run_test "Integer literal" "42;" 42
run_test "Zero literal" "0;" 0
run_test "Negative literal" "-5;" 251  # 256-5 due to 8-bit exit codes

# Basic arithmetic
run_test "Addition" "2 + 3;" 5
run_test "Subtraction" "10 - 3;" 7
run_test "Multiplication" "4 * 6;" 24
run_test "Division" "15 / 3;" 5
run_test "Modulo" "17 % 5;" 2

# Operator precedence
run_test "Precedence 1" "2 + 3 * 4;" 14
run_test "Precedence 2" "20 - 2 * 3;" 14
run_test "Precedence 3" "(2 + 3) * 4;" 20
run_test "Complex precedence" "2 + 3 * 4 - 1;" 13

# ==============================================
# PHASE 2: COMPARISON OPERATIONS
# ==============================================
echo "=== PHASE 2: COMPARISON OPERATIONS ==="

run_test "Equal true" "5 == 5;" 1
run_test "Equal false" "5 == 3;" 0
run_test "Not equal true" "5 != 3;" 1
run_test "Not equal false" "5 != 5;" 0
run_test "Less than true" "3 < 5;" 1
run_test "Less than false" "5 < 3;" 0
run_test "Greater than true" "5 > 3;" 1
run_test "Greater than false" "3 > 5;" 0
run_test "Less equal true" "3 <= 5;" 1
run_test "Less equal equal" "5 <= 5;" 1
run_test "Greater equal true" "5 >= 3;" 1
run_test "Greater equal equal" "5 >= 5;" 1

# ==============================================
# PHASE 3: LOGICAL OPERATIONS
# ==============================================
echo "=== PHASE 3: LOGICAL OPERATIONS ==="

run_test "AND true" "1 && 1;" 1
run_test "AND false" "1 && 0;" 0
run_test "OR true" "1 || 0;" 1
run_test "OR false" "0 || 0;" 0
run_test "NOT true" "!0;" 1
run_test "NOT false" "!1;" 0

# ==============================================
# PHASE 4: VARIABLES
# ==============================================
echo "=== PHASE 4: VARIABLE DECLARATIONS ==="

run_test "Variable declaration" "int x = 5; x;" 5
run_test "Variable assignment" "int x = 3; x = 7; x;" 7
run_test "Multiple variables" "int a = 2; int b = 3; a + b;" 5
run_test "Variable in expression" "int x = 4; x * 2 + 1;" 9

# ==============================================
# PHASE 5: CONTROL FLOW (if available)
# ==============================================
echo "=== PHASE 5: CONTROL FLOW STATEMENTS ==="

# These may fail if not implemented yet
run_test "If statement true" "int x = 5; if (x > 3) x = 10; x;" 10
run_test "If statement false" "int x = 2; if (x > 3) x = 10; x;" 2
run_test "If-else true" "int x = 5; if (x > 3) x = 10; else x = 1; x;" 10
run_test "If-else false" "int x = 2; if (x > 3) x = 10; else x = 1; x;" 1

# ==============================================
# PHASE 6: LOOPS (if available)
# ==============================================
echo "=== PHASE 6: LOOP STATEMENTS ==="

run_test "While loop" "int x = 0; while (x < 3) x = x + 1; x;" 3
run_test "For loop" "int i = 0; for (i = 0; i < 5; i = i + 1) ; i;" 5

# ==============================================
# PHASE 7: I/O STATEMENTS (if available)
# ==============================================
echo "=== PHASE 7: I/O STATEMENTS ==="

# These might be harder to test automatically
echo "Note: I/O statements (cout/cin) require manual testing"

# ==============================================
# PHASE 8: ERROR CASES
# ==============================================
echo "=== PHASE 8: ERROR HANDLING ==="

test_error() {
    local test_name="$1"
    local input_code="$2"

    echo -e "${YELLOW}Testing Error: $test_name${NC}"
    echo "Code: $input_code"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    echo "$input_code" > test_temp.cpp

    if ./compiler test_temp.cpp -o test_temp.s 2>/dev/null; then
        echo -e "${RED}❌ FAIL: Should have failed compilation${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    else
        echo -e "${GREEN}✅ PASS: Correctly rejected invalid code${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    fi

    echo ""
    rm -f test_temp.cpp test_temp.s test_temp.o test_temp_exe
}

test_error "Missing semicolon" "2 + 3"
test_error "Undefined variable" "x + 5;"
test_error "Invalid operator" "2 @ 3;"

# ==============================================
# SUMMARY
# ==============================================
echo "==============================================="
echo "                TEST SUMMARY"
echo "==============================================="
echo "Total Tests: $TOTAL_TESTS"
echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed: ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}🎉 ALL TESTS PASSED!${NC}"
    echo "Your compiler is working excellently!"
else
    echo -e "${YELLOW}⚠️  Some tests failed.${NC}"
    echo "This is normal for a compiler in development."
    echo "Focus on implementing the failed features next."
fi

echo ""
echo "Success rate: $(( (PASSED_TESTS * 100) / TOTAL_TESTS ))%"
echo ""

# Feature implementation suggestions
echo "=== NEXT STEPS BASED ON TEST RESULTS ==="
echo ""
echo "1. If basic expressions work: ✅ Core parser is solid"
echo "2. If variables work: ✅ Symbol table is working"
echo "3. If control flow fails: Implement if/while/for statements"
echo "4. If comparisons fail: Check comparison operators in codegen"
echo "5. If logical ops fail: Implement &&, ||, ! operators"
echo ""
echo "Use './compiler --verbose test.cpp' to debug specific failures"
echo "Use './compiler --ast-only test.cpp' to check AST generation"
