#!/bin/bash

# Implementation Status Checker
# Tests individual features to see what's implemented

echo "================================================"
echo "   C++ Compiler Implementation Status Check"
echo "================================================"

# Check if compiler exists
if [ ! -f "./compiler" ]; then
    echo "❌ Compiler not found. Run 'make' first."
    exit 1
fi

echo "Testing individual language features..."
echo ""

# Function to test a feature
test_feature() {
    local feature_name="$1"
    local test_code="$2"

    echo -n "Testing $feature_name: "
    echo "$test_code" > feature_test.cpp

    if ./compiler feature_test.cpp -o feature_test.s 2>/dev/null; then
        if as -64 feature_test.s -o feature_test.o 2>/dev/null; then
            if ld feature_test.o -o feature_test_exe 2>/dev/null; then
                ./feature_test_exe 2>/dev/null
                local result=$?
                echo "✅ WORKING (result: $result)"
                rm -f feature_test.cpp feature_test.s feature_test.o feature_test_exe
                return 0
            fi
        fi
    fi

    echo "❌ NOT WORKING"
    rm -f feature_test.cpp feature_test.s feature_test.o feature_test_exe
    return 1
}

# Test AST-only mode for features that might not generate code yet
test_ast_only() {
    local feature_name="$1"
    local test_code="$2"

    echo -n "Testing $feature_name (AST): "
    echo "$test_code" > feature_test.cpp

    if ./compiler --ast-only feature_test.cpp 2>/dev/null >/dev/null; then
        echo "✅ PARSED (AST generation works)"
        rm -f feature_test.cpp
        return 0
    else
        echo "❌ PARSE ERROR"
        rm -f feature_test.cpp
        return 1
    fi
}

echo "=== CORE FEATURES ==="
test_feature "Basic arithmetic" "2 + 3 * 4;"
test_feature "Parentheses" "(2 + 3) * 4;"
test_feature "Variable declaration" "int x = 5; x;"
test_feature "Variable assignment" "int x = 3; x = 7; x;"

echo ""
echo "=== COMPARISON OPERATORS ==="
test_feature "Equality" "5 == 5;"
test_feature "Not equal" "5 != 3;"
test_feature "Less than" "3 < 5;"
test_feature "Greater than" "5 > 3;"

echo ""
echo "=== LOGICAL OPERATORS ==="
test_feature "Logical AND" "1 && 1;"
test_feature "Logical OR" "1 || 0;"
test_feature "Logical NOT" "!0;"

echo ""
echo "=== CONTROL FLOW ==="
test_ast_only "If statement" "int x = 5; if (x > 3) x = 10;"
test_ast_only "If-else statement" "int x = 2; if (x > 3) x = 10; else x = 1;"
test_ast_only "While loop" "int x = 0; while (x < 3) x = x + 1;"
test_ast_only "For loop" "for (int i = 0; i < 5; i = i + 1) ;"

echo ""
echo "=== I/O STATEMENTS ==="
test_ast_only "Cout statement" 'cout << "Hello";'
test_ast_only "Cin statement" "int x; cin >> x;"

echo ""
echo "=== COMPOUND STATEMENTS ==="
test_ast_only "Block statement" "{ int x = 5; int y = 3; }"
test_ast_only "Nested blocks" "{ int x = 5; { int y = 3; } }"

echo ""
echo "=== DATA TYPES ==="
test_ast_only "Float literal" "3.14;"
test_ast_only "String literal" '"Hello world";'
test_ast_only "Character literal" "'a';"
test_ast_only "Boolean literal" "true;"

echo ""
echo "================================================"
echo "            IMPLEMENTATION SUMMARY"
echo "================================================"
echo ""
echo "Based on the results above:"
echo ""
echo "✅ Working features can be used in test programs"
echo "🔄 AST-parsed features need code generation"
echo "❌ Failed features need parser implementation"
echo ""
echo "NEXT STEPS:"
echo "1. Focus on features that parse but don't generate code"
echo "2. Implement code generation for control flow statements"
echo "3. Add error handling for failed parsing features"
echo ""
echo "Run the full test suite with: ./test_suite.sh"
echo "Debug specific features with: ./compiler --verbose --ast-only test.cpp"
