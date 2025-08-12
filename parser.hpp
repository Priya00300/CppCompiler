#ifndef PARSER_HPP
#define PARSER_HPP

#include "scanner.hpp"
#include "tokens.hpp"
#include <memory>
#include <vector>
#include <iostream>

// Forward declaration
class Scanner;
class StringScanner;

// AST Node Types - Enhanced with more statement types
enum class ASTNodeType {
    // Literals
    INTLIT,
    FLOATLIT,
    STRINGLIT,
    CHARLIT,
    BOOLLIT,

    // Identifiers
    IDENTIFIER,

    // Binary operations
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,

    // Comparison operations
    EQ,          // ==
    NE,          // !=
    LT,          // <
    GT,          // >
    LE,          // <=
    GE,          // >=

    // Logical operations
    AND,         // &&
    OR,          // ||
    NOT,         // !

    // Unary operations
    NEGATE,      // -
    POSITIVE,    // +

    // Assignment
    ASSIGN,      // =

    // Statements
    VAR_DECL,           // int x;
    EXPRESSION_STMT,    // expression;
    COMPOUND_STMT,      // { ... }
    IF_STMT,           // if (condition) statement
    WHILE_STMT,        // while (condition) statement
    FOR_STMT,          // for (init; condition; update) statement
    RETURN_STMT,       // return expression;

    // I/O Statements
    COUT_STMT,         // cout << expression;
    CIN_STMT,          // cin >> variable;

    // Program structure
    PROGRAM,           // Root node
    BLOCK              // Block of statements
};

// AST Node structure
struct ASTNode {
    ASTNodeType type;
    std::string value;           // For identifiers, literals
    int intValue;               // For integer literals
    float floatValue;           // For float literals
    bool boolValue;            // For boolean literals

    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::unique_ptr<ASTNode> condition;  // For if/while statements
    std::vector<std::unique_ptr<ASTNode>> children;  // For compound statements

    // Constructor
    ASTNode(ASTNodeType t) : type(t), intValue(0), floatValue(0.0f), boolValue(false) {}

    // Copy constructor and assignment operator deleted to prevent issues
    ASTNode(const ASTNode&) = delete;
    ASTNode& operator=(const ASTNode&) = delete;
};

// Parser class
class Parser {
private:
    Scanner* scanner;
    Token currentToken;
    bool ownedScanner;  // Whether we own the scanner

    // Token handling
    void nextToken();
    void expectToken(TokenType expected);
    bool matchToken(TokenType expected);

    // Expression parsing (Pratt parser)
    std::unique_ptr<ASTNode> parseExpression(int minPrecedence = 0);
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseUnary();

    // Statement parsing
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseVariableDeclaration();
    std::unique_ptr<ASTNode> parseExpressionStatement();
    std::unique_ptr<ASTNode> parseCompoundStatement();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseWhileStatement();
    std::unique_ptr<ASTNode> parseForStatement();
    std::unique_ptr<ASTNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parseCoutStatement();
    std::unique_ptr<ASTNode> parseCinStatement();

    // Program parsing
    std::unique_ptr<ASTNode> parseProgram();

    // Precedence handling
    int getOperatorPrecedence(TokenType tokenType);
    bool isRightAssociative(TokenType tokenType);
    ASTNodeType tokenToASTNode(TokenType tokenType);

    // Error handling
    void error(const std::string& message);
    void synchronize(); // Error recovery

public:
    // Constructors
    explicit Parser(Scanner* s);
    explicit Parser(const std::string& input); // For string parsing
    ~Parser();

    // Main parsing methods
    std::unique_ptr<ASTNode> parse();
    std::unique_ptr<ASTNode> parseExpressionOnly();

    // Utility methods
    void printAST(const std::unique_ptr<ASTNode>& node, int depth = 0);
    std::string astNodeTypeToString(ASTNodeType type);
};

// String scanner for parsing expressions from strings
class StringScanner : public Scanner {
private:
    std::string input;
    size_t position;
    int line;
    int column;
    std::unordered_map<std::string, TokenType> keywords;

public:
    StringScanner(const std::string& text);
    Token getNextToken() override;
    Token peekToken() override;
    int getCurrentLine() const override { return line; }
    int getCurrentColumn() const override { return column; }

private:
    void skipWhitespace();
    Token scanNumber();
    Token scanString();
    Token scanCharacter();
    Token scanIdentifier();
    Token scanOperator();
    bool isAtEnd() const { return position >= input.length(); }
    char currentChar() const { return isAtEnd() ? '\0' : input[position]; }
    char peekChar() const { return (position + 1 >= input.length()) ? '\0' : input[position + 1]; }
    void advance() { if (!isAtEnd()) { position++; column++; } }
    void initializeKeywords();
};

#endif // PARSER_HPP
