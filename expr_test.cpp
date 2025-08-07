#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "tokens.hpp"
#include "scanner.hpp"
#include "parser.hpp"

// Simple scanner adapter for string input (for testing expressions)
// NOW INHERITS FROM Scanner to be compatible with Parser
class StringScanner : public Scanner {
private:
    std::istringstream input;
    std::string line;
    size_t pos;
    int lineNum;
    int colNum;
    char currentChar;

public:
    StringScanner(const std::string& expr) : input(expr), pos(0), lineNum(1), colNum(1) {
        line = expr;
        currentChar = pos < line.length() ? line[pos] : '\0';
    }

    // Override the virtual method from Scanner
    Token getNextToken() override {
        // Skip whitespace
        while (currentChar == ' ' || currentChar == '\t') {
            nextChar();
        }

        // End of input
        if (currentChar == '\0' || currentChar == ';') {
            if (currentChar == ';') nextChar();
            return Token(TokenType::T_EOF, "", lineNum, colNum);
        }

        // Numbers
        if (isdigit(currentChar)) {
            return scanNumber();
        }

        // Identifiers
        if (isalpha(currentChar) || currentChar == '_') {
            return scanIdentifier();
        }

        // Operators and punctuation
        return scanOperator();
    }

    // Override peekToken as well
    Token peekToken() override {
        // Save current state
        size_t savedPos = pos;
        int savedLine = lineNum;
        int savedCol = colNum;
        char savedChar = currentChar;

        // Get next token
        Token token = getNextToken();

        // Restore state
        pos = savedPos;
        lineNum = savedLine;
        colNum = savedCol;
        currentChar = savedChar;

        return token;
    }

    // Override position methods
    int getCurrentLine() const override { return lineNum; }
    int getCurrentColumn() const override { return colNum; }

private:
    void nextChar() {
        if (pos < line.length()) {
            pos++;
            colNum++;
            currentChar = pos < line.length() ? line[pos] : '\0';
        }
    }

    Token scanNumber() {
        std::string number;
        int startCol = colNum;

        while (isdigit(currentChar)) {
            number += currentChar;
            nextChar();
        }

        // Check for float
        if (currentChar == '.') {
            number += currentChar;
            nextChar();
            while (isdigit(currentChar)) {
                number += currentChar;
                nextChar();
            }
            return Token(TokenType::T_FLOATLIT, number, lineNum, startCol);
        }

        return Token(TokenType::T_INTLIT, number, lineNum, startCol);
    }

    Token scanIdentifier() {
        std::string ident;
        int startCol = colNum;

        while (isalnum(currentChar) || currentChar == '_') {
            ident += currentChar;
            nextChar();
        }

        // Check if it's a keyword
        auto it = keywords.find(ident);
        if (it != keywords.end()) {
            return Token(it->second, ident, lineNum, startCol);
        }

        return Token(TokenType::T_IDENT, ident, lineNum, startCol);
    }

    Token scanOperator() {
        int startCol = colNum;
        char ch = currentChar;
        nextChar();

        switch (ch) {
            case '+':
                if (currentChar == '+') { nextChar(); return Token(TokenType::T_INCREMENT, "++", lineNum, startCol); }
                if (currentChar == '=') { nextChar(); return Token(TokenType::T_PLUSEQ, "+=", lineNum, startCol); }
                return Token(TokenType::T_PLUS, "+", lineNum, startCol);
            case '-':
                if (currentChar == '-') { nextChar(); return Token(TokenType::T_DECREMENT, "--", lineNum, startCol); }
                if (currentChar == '=') { nextChar(); return Token(TokenType::T_MINUSEQ, "-=", lineNum, startCol); }
                return Token(TokenType::T_MINUS, "-", lineNum, startCol);
            case '*':
                if (currentChar == '=') { nextChar(); return Token(TokenType::T_STAREQ, "*=", lineNum, startCol); }
                return Token(TokenType::T_STAR, "*", lineNum, startCol);
            case '/':
                if (currentChar == '=') { nextChar(); return Token(TokenType::T_SLASHEQ, "/=", lineNum, startCol); }
                return Token(TokenType::T_SLASH, "/", lineNum, startCol);
            case '%': return Token(TokenType::T_PERCENT, "%", lineNum, startCol);
            case '=':
                if (currentChar == '=') { nextChar(); return Token(TokenType::T_EQ, "==", lineNum, startCol); }
                return Token(TokenType::T_ASSIGN, "=", lineNum, startCol);
            case '!':
                if (currentChar == '=') { nextChar(); return Token(TokenType::T_NE, "!=", lineNum, startCol); }
                return Token(TokenType::T_NOT, "!", lineNum, startCol);
            case '<':
                if (currentChar == '=') { nextChar(); return Token(TokenType::T_LE, "<=", lineNum, startCol); }
                if (currentChar == '<') { nextChar(); return Token(TokenType::T_LSHIFT, "<<", lineNum, startCol); }
                return Token(TokenType::T_LT, "<", lineNum, startCol);
            case '>':
                if (currentChar == '=') { nextChar(); return Token(TokenType::T_GE, ">=", lineNum, startCol); }
                if (currentChar == '>') { nextChar(); return Token(TokenType::T_RSHIFT, ">>", lineNum, startCol); }
                return Token(TokenType::T_GT, ">", lineNum, startCol);
            case '&':
                if (currentChar == '&') { nextChar(); return Token(TokenType::T_AND, "&&", lineNum, startCol); }
                return Token(TokenType::T_BITAND, "&", lineNum, startCol);
            case '|':
                if (currentChar == '|') { nextChar(); return Token(TokenType::T_OR, "||", lineNum, startCol); }
                return Token(TokenType::T_BITOR, "|", lineNum, startCol);
            case '^': return Token(TokenType::T_BITXOR, "^", lineNum, startCol);
            case '~': return Token(TokenType::T_BITNOT, "~", lineNum, startCol);
            case '(': return Token(TokenType::T_LPAREN, "(", lineNum, startCol);
            case ')': return Token(TokenType::T_RPAREN, ")", lineNum, startCol);
            case ';': return Token(TokenType::T_SEMICOLON, ";", lineNum, startCol);
            default:
                return Token(TokenType::T_ERROR, std::string(1, ch), lineNum, startCol);
        }
    }
};

void testExpression(const std::string& expr) {
    std::cout << "\n=== Testing Expression: " << expr << " ===" << std::endl;

    try {
        StringScanner scanner(expr);
        Parser parser(&scanner);

        auto ast = parser.parseExpressionOnly();

        std::cout << "✓ Parse successful!" << std::endl;
        std::cout << "AST Structure:" << std::endl;
        parser.printAST(ast, 1);

    } catch (const std::exception& e) {
        std::cout << "❌ Parse failed: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Initialize keywords
    initializeKeywords();

    if (argc == 2) {
        // Test single expression from command line
        testExpression(argv[1]);
        return 0;
    }

    std::cout << "=== C++ Compiler - Expression Precedence Tester ===" << std::endl;
    std::cout << "Testing operator precedence with Pratt parser implementation" << std::endl;

    // Test cases demonstrating operator precedence
    std::vector<std::string> testCases = {
        // Basic arithmetic precedence
        "2 + 3 * 4",           // Should be: 2 + (3 * 4)
        "2 * 3 + 4",           // Should be: (2 * 3) + 4
        "2 + 3 * 4 + 5",       // Should be: 2 + (3 * 4) + 5
        "2 * 3 + 4 * 5",       // Should be: (2 * 3) + (4 * 5)

        // Parentheses
        "(2 + 3) * 4",         // Should respect parentheses
        "2 * (3 + 4)",         // Should respect parentheses
        "((2 + 3) * 4) + 5",   // Nested parentheses

        // Unary operators
        "-2 + 3",              // Should be: (-2) + 3
        "2 + -3",              // Should be: 2 + (-3)
        "-2 * -3",             // Should be: (-2) * (-3)

        // Comparison operators
        "2 + 3 < 4 * 2",       // Should be: (2 + 3) < (4 * 2)
        "a == b + c",          // Should be: a == (b + c)
        "a + b == c * d",      // Should be: (a + b) == (c * d)

        // Logical operators
        "a && b || c",         // Should be: (a && b) || c
        "a || b && c",         // Should be: a || (b && c)
        "!a && b",             // Should be: (!a) && b

        // Assignment (right-associative)
        "a = b = c",           // Should be: a = (b = c)
        "a = b + c",           // Should be: a = (b + c)

        // Complex mixed expressions
        "a = b + c * d == e && f || g",  // Complex precedence test
        "x + y * z > a && b || c = d",   // Another complex test

        // Error cases
        "2 + + 3",             // Should handle error gracefully
        "2 * * 3",             // Should handle error gracefully
    };

    for (const auto& testCase : testCases) {
        testExpression(testCase);
    }

    // Interactive mode
    std::cout << "\n=== Interactive Mode ===" << std::endl;
    std::cout << "Enter expressions to test (or 'quit' to exit):" << std::endl;

    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "quit" || input == "exit" || input == "q") {
            break;
        }

        if (!input.empty()) {
            testExpression(input);
        }
    }

    std::cout << "Thanks for testing the precedence parser!" << std::endl;
    return 0;
}

/*
Usage examples:

1. Compile:
   g++ -std=c++17 -Wall -Wextra -g expr_test.cpp tokens.cpp -o expr_test

2. Run predefined tests:
   ./expr_test

3. Test single expression:
   ./expr_test "2 + 3 * 4"

4. Expected output for "2 + 3 * 4":
   === Testing Expression: 2 + 3 * 4 ===
   ✓ Parse successful!
   AST Structure:
     ADD
       INTLIT: 2
       MULTIPLY
         INTLIT: 3
         INTLIT: 4

   This shows correct precedence: multiplication before addition
*/
