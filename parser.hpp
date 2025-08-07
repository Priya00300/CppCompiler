#ifndef PARSER_HPP
#define PARSER_HPP

#include "tokens.hpp"
#include "scanner.hpp"
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

// AST Node Types
enum class ASTNodeType {
    // Literals
    INTLIT,
    FLOATLIT,
    STRINGLIT,
    CHARLIT,
    IDENTIFIER,

    // Binary Operations - Arithmetic
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,

    // Assignment
    ASSIGN,

    // Comparison operators
    EQ,         // ==
    NE,         // !=
    LT,         // <
    GT,         // >
    LE,         // <=
    GE,         // >=

    // Logical operators
    AND,        // &&
    OR,         // ||
    NOT,        // !

    // Unary operations
    NEGATE,     // unary -
    POSITIVE,   // unary +

    // Statements
    EXPRESSION_STMT,
    COUT_STMT,
    CIN_STMT,
    IF_STMT,
    WHILE_STMT,
    FOR_STMT,
    RETURN_STMT,
    COMPOUND_STMT,  // { ... }

    // Declarations
    VAR_DECL,

    // Program structure
    PROGRAM
};

// Forward declaration
struct ASTNode;

// AST Node structure
struct ASTNode {
    ASTNodeType type;
    std::string value;          // For literals and identifiers
    int intValue;               // For integer literals
    float floatValue;           // For float literals

    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::vector<std::unique_ptr<ASTNode>> children;  // For statements with multiple children

    // Constructor for leaf nodes (literals, identifiers)
    ASTNode(ASTNodeType t, const std::string& v = "", int iv = 0, float fv = 0.0f)
        : type(t), value(v), intValue(iv), floatValue(fv), left(nullptr), right(nullptr) {}

    // Constructor for binary operations
    ASTNode(ASTNodeType t, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : type(t), value(""), intValue(0), floatValue(0.0f), left(std::move(l)), right(std::move(r)) {}

    // Constructor for unary operations
    ASTNode(ASTNodeType t, std::unique_ptr<ASTNode> child)
        : type(t), value(""), intValue(0), floatValue(0.0f), left(std::move(child)), right(nullptr) {}
};

class Parser {
private:
    Scanner* scanner;
    Token currentToken;
    bool hasCurrentToken;

    // Operator precedence table - higher numbers = higher precedence
    static const int precedenceTable[];

    // Error handling
    void error(const std::string& message);
    void unexpectedToken(const std::string& expected = "");

    // Token management
    void nextToken();
    void expectToken(TokenType expected);
    bool matchToken(TokenType expected);

    // AST creation helpers
    std::unique_ptr<ASTNode> makeLeafNode(ASTNodeType type, const std::string& value = "",
                                         int intVal = 0, float floatVal = 0.0f);
    std::unique_ptr<ASTNode> makeBinaryNode(ASTNodeType type, std::unique_ptr<ASTNode> left,
                                           std::unique_ptr<ASTNode> right);
    std::unique_ptr<ASTNode> makeUnaryNode(ASTNodeType type, std::unique_ptr<ASTNode> child);

    // Precedence and operator handling
    int getOperatorPrecedence(TokenType tokenType);
    bool isOperator(TokenType tokenType);
    bool isRightAssociative(TokenType tokenType);
    ASTNodeType tokenToASTType(TokenType tokenType);

    // Pratt parsing - core expression parsing
    std::unique_ptr<ASTNode> parseExpression(int minPrecedence = 0);
    std::unique_ptr<ASTNode> parsePrimaryExpression();
    std::unique_ptr<ASTNode> parseUnaryExpression();

    // Statement parsing methods
    std::unique_ptr<ASTNode> parseProgram();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpressionStatement();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<ASTNode> parseCoutStatement();
    std::unique_ptr<ASTNode> parseCinStatement();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ASTNode> parseWhileStatement();
    std::unique_ptr<ASTNode> parseForStatement();
    std::unique_ptr<ASTNode> parseReturnStatement();
    std::unique_ptr<ASTNode> parseCompoundStatement();

public:
    Parser(Scanner* scan);
    ~Parser() = default;

    // Main parsing entry point
    std::unique_ptr<ASTNode> parse();

    // Debug and utility methods
    void printAST(const std::unique_ptr<ASTNode>& node, int indent = 0);
    std::string astNodeTypeToString(ASTNodeType type);

    // Test expression parsing specifically
    std::unique_ptr<ASTNode> parseExpressionOnly();
};

#endif // PARSER_HPP
