#include "parser.hpp"
#include <iostream>
#include <iomanip>

// Operator precedence table - follows C++ operator precedence
// Higher numbers = higher precedence
const int Parser::precedenceTable[] = {
    0,   // T_EOF
    0,   // T_INTLIT
    0,   // T_FLOATLIT
    0,   // T_STRINGLIT
    0,   // T_CHARLIT
    0,   // T_IDENT

    // Data types
    0,   // T_INT
    0,   // T_FLOAT
    0,   // T_CHAR
    0,   // T_DOUBLE
    0,   // T_BOOL
    0,   // T_VOID

    // Keywords
    0,   // T_IF
    0,   // T_ELSE
    0,   // T_WHILE
    0,   // T_FOR
    0,   // T_RETURN
    0,   // T_COUT
    0,   // T_CIN
    0,   // T_ENDL
    0,   // T_TRUE
    0,   // T_FALSE
    0,   // T_CONST
    0,   // T_CLASS
    0,   // T_PUBLIC
    0,   // T_PRIVATE
    0,   // T_PROTECTED
    0,   // T_NAMESPACE
    0,   // T_STD
    0,   // T_USING
    0,   // T_INCLUDE

    // Arithmetic operators (precedence 10-40)
    30,  // T_PLUS      +
    30,  // T_MINUS     -
    40,  // T_STAR      *
    40,  // T_SLASH     /
    40,  // T_PERCENT   %

    // Assignment (precedence 2)
    2,   // T_ASSIGN    =

    // Comparison operators (precedence 20-25)
    20,  // T_EQ        ==
    20,  // T_NE        !=
    25,  // T_LT        <
    25,  // T_GT        >
    25,  // T_LE        <=
    25,  // T_GE        >=

    // Logical operators (precedence 5-15)
    5,   // T_AND       &&
    3,   // T_OR        ||
    0,   // T_NOT       ! (unary)

    // Bitwise operators (precedence 15-35)
    15,  // T_BITAND    &
    13,  // T_BITOR     |
    14,  // T_BITXOR    ^
    0,   // T_BITNOT    ~ (unary)
    35,  // T_LSHIFT    <<
    35,  // T_RSHIFT    >>

    // Increment/Decrement (precedence 50)
    50,  // T_INCREMENT ++
    50,  // T_DECREMENT --

    // Compound assignment (precedence 2)
    2,   // T_PLUSEQ    +=
    2,   // T_MINUSEQ   -=
    2,   // T_STAREQ    *=
    2,   // T_SLASHEQ   /=

    // Special operators
    0,   // T_ARROW     ->
    0,   // T_SCOPE     ::

    // Delimiters
    0,   // T_SEMICOLON ;
    0,   // T_COMMA     ,
    0,   // T_LPAREN    (
    0,   // T_RPAREN    )
    0,   // T_LBRACE    {
    0,   // T_RBRACE    }
    0,   // T_LBRACKET  [
    0,   // T_RBRACKET  ]
    0,   // T_COLON     :
    0,   // T_DOT       .
    0,   // T_QUESTION  ?

    // Special tokens
    0,   // T_HASH      #
    0,   // T_NEWLINE
    0,   // T_WHITESPACE
    0,   // T_COMMENT
    0    // T_ERROR
};

Parser::Parser(Scanner* scan) : scanner(scan), hasCurrentToken(false) {
    // Initialize keywords if not already done
    initializeKeywords();
    // Get the first token
    nextToken();
}

void Parser::error(const std::string& message) {
    std::cerr << "Parser Error at line " << currentToken.line
              << ", column " << currentToken.column << ": " << message << std::endl;
    throw std::runtime_error("Parse error: " + message);
}

void Parser::unexpectedToken(const std::string& expected) {
    std::string message = "Unexpected token '" + currentToken.value + "'";
    if (!expected.empty()) {
        message += ", expected " + expected;
    }
    error(message);
}

void Parser::nextToken() {
    do {
        currentToken = scanner->getNextToken();
        hasCurrentToken = true;
    } while (currentToken.type == TokenType::T_WHITESPACE ||
             currentToken.type == TokenType::T_NEWLINE ||
             currentToken.type == TokenType::T_COMMENT);
}

void Parser::expectToken(TokenType expected) {
    if (currentToken.type != expected) {
        unexpectedToken(getTokenTypeName(expected));
    }
    nextToken();
}

bool Parser::matchToken(TokenType expected) {
    if (currentToken.type == expected) {
        nextToken();
        return true;
    }
    return false;
}

std::unique_ptr<ASTNode> Parser::makeLeafNode(ASTNodeType type, const std::string& value,
                                             int intVal, float floatVal) {
    return std::make_unique<ASTNode>(type, value, intVal, floatVal);
}

std::unique_ptr<ASTNode> Parser::makeBinaryNode(ASTNodeType type, std::unique_ptr<ASTNode> left,
                                               std::unique_ptr<ASTNode> right) {
    return std::make_unique<ASTNode>(type, std::move(left), std::move(right));
}

std::unique_ptr<ASTNode> Parser::makeUnaryNode(ASTNodeType type, std::unique_ptr<ASTNode> child) {
    return std::make_unique<ASTNode>(type, std::move(child));
}

int Parser::getOperatorPrecedence(TokenType tokenType) {
    int tokenIndex = static_cast<int>(tokenType);
    size_t tableSize = sizeof(precedenceTable) / sizeof(precedenceTable[0]);

    if (tokenIndex >= 0 && static_cast<size_t>(tokenIndex) < tableSize) {
        return precedenceTable[tokenIndex];
    }
    return 0;
}

bool Parser::isOperator(TokenType tokenType) {
    return getOperatorPrecedence(tokenType) > 0;
}

bool Parser::isRightAssociative(TokenType tokenType) {
    // Assignment operators are right-associative
    return tokenType == TokenType::T_ASSIGN ||
           tokenType == TokenType::T_PLUSEQ ||
           tokenType == TokenType::T_MINUSEQ ||
           tokenType == TokenType::T_STAREQ ||
           tokenType == TokenType::T_SLASHEQ;
}

ASTNodeType Parser::tokenToASTType(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::T_PLUS:     return ASTNodeType::ADD;
        case TokenType::T_MINUS:    return ASTNodeType::SUBTRACT;
        case TokenType::T_STAR:     return ASTNodeType::MULTIPLY;
        case TokenType::T_SLASH:    return ASTNodeType::DIVIDE;
        case TokenType::T_PERCENT:  return ASTNodeType::MODULO;
        case TokenType::T_ASSIGN:   return ASTNodeType::ASSIGN;
        case TokenType::T_EQ:       return ASTNodeType::EQ;
        case TokenType::T_NE:       return ASTNodeType::NE;
        case TokenType::T_LT:       return ASTNodeType::LT;
        case TokenType::T_GT:       return ASTNodeType::GT;
        case TokenType::T_LE:       return ASTNodeType::LE;
        case TokenType::T_GE:       return ASTNodeType::GE;
        case TokenType::T_AND:      return ASTNodeType::AND;
        case TokenType::T_OR:       return ASTNodeType::OR;
        default:
            error("Invalid token type for AST conversion: " + getTokenTypeName(tokenType));
            return ASTNodeType::ADD; // This won't be reached due to error
    }
}

// Main parsing entry point
std::unique_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

// Pratt parser implementation for expressions
std::unique_ptr<ASTNode> Parser::parseExpression(int minPrecedence) {
    // Parse the left-hand side (could be primary or unary expression)
    auto left = parseUnaryExpression();

    // While we have operators with sufficient precedence
    while (isOperator(currentToken.type) &&
           getOperatorPrecedence(currentToken.type) >= minPrecedence) {

        TokenType operatorToken = currentToken.type;
        int operatorPrec = getOperatorPrecedence(operatorToken);

        nextToken(); // consume the operator

        // For right-associative operators, use the same precedence
        // For left-associative operators, use precedence + 1
        int nextMinPrec = isRightAssociative(operatorToken) ? operatorPrec : operatorPrec + 1;

        // Parse the right-hand side with appropriate precedence
        auto right = parseExpression(nextMinPrec);

        // Create binary operator node
        left = makeBinaryNode(tokenToASTType(operatorToken), std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnaryExpression() {
    // Handle unary operators
    if (currentToken.type == TokenType::T_MINUS) {
        nextToken();
        auto expr = parseUnaryExpression();
        return makeUnaryNode(ASTNodeType::NEGATE, std::move(expr));
    }

    if (currentToken.type == TokenType::T_PLUS) {
        nextToken();
        auto expr = parseUnaryExpression();
        return makeUnaryNode(ASTNodeType::POSITIVE, std::move(expr));
    }

    if (currentToken.type == TokenType::T_NOT) {
        nextToken();
        auto expr = parseUnaryExpression();
        return makeUnaryNode(ASTNodeType::NOT, std::move(expr));
    }

    return parsePrimaryExpression();
}

std::unique_ptr<ASTNode> Parser::parsePrimaryExpression() {
    switch (currentToken.type) {
        case TokenType::T_INTLIT: {
            int value = std::stoi(currentToken.value);
            nextToken();
            return makeLeafNode(ASTNodeType::INTLIT, "", value);
        }

        case TokenType::T_FLOATLIT: {
            float value = std::stof(currentToken.value);
            nextToken();
            return makeLeafNode(ASTNodeType::FLOATLIT, "", 0, value);
        }

        case TokenType::T_STRINGLIT: {
            std::string value = currentToken.value;
            nextToken();
            return makeLeafNode(ASTNodeType::STRINGLIT, value);
        }

        case TokenType::T_CHARLIT: {
            std::string value = currentToken.value;
            nextToken();
            return makeLeafNode(ASTNodeType::CHARLIT, value);
        }

        case TokenType::T_IDENT: {
            std::string name = currentToken.value;
            nextToken();
            return makeLeafNode(ASTNodeType::IDENTIFIER, name);
        }

        case TokenType::T_TRUE: {
            nextToken();
            return makeLeafNode(ASTNodeType::INTLIT, "true", 1);
        }

        case TokenType::T_FALSE: {
            nextToken();
            return makeLeafNode(ASTNodeType::INTLIT, "false", 0);
        }

        case TokenType::T_LPAREN: {
            nextToken(); // consume '('
            auto expr = parseExpression(0); // Start with minimum precedence
            expectToken(TokenType::T_RPAREN);
            return expr;
        }

        default:
            unexpectedToken("primary expression");
            return nullptr; // This won't be reached due to error
    }
}

// Test method for expression parsing only
std::unique_ptr<ASTNode> Parser::parseExpressionOnly() {
    auto expr = parseExpression(0);
    if (currentToken.type == TokenType::T_SEMICOLON) {
        nextToken();
    }
    if (currentToken.type != TokenType::T_EOF) {
        error("Expected end of expression");
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseProgram() {
    auto program = makeLeafNode(ASTNodeType::PROGRAM);

    while (currentToken.type != TokenType::T_EOF) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->children.push_back(std::move(stmt));
            }
        } catch (const std::runtime_error& e) {
            // Skip to next statement on error
            while (currentToken.type != TokenType::T_SEMICOLON &&
                   currentToken.type != TokenType::T_EOF) {
                nextToken();
            }
            if (currentToken.type == TokenType::T_SEMICOLON) {
                nextToken();
            }
        }
    }

    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    switch (currentToken.type) {
        case TokenType::T_INT:
        case TokenType::T_FLOAT:
        case TokenType::T_CHAR:
        case TokenType::T_DOUBLE:
        case TokenType::T_BOOL:
            return parseDeclaration();

        case TokenType::T_COUT:
            return parseCoutStatement();

        case TokenType::T_CIN:
            return parseCinStatement();

        case TokenType::T_IF:
            return parseIfStatement();

        case TokenType::T_WHILE:
            return parseWhileStatement();

        case TokenType::T_FOR:
            return parseForStatement();

        case TokenType::T_RETURN:
            return parseReturnStatement();

        case TokenType::T_LBRACE:
            return parseCompoundStatement();

        default:
            return parseExpressionStatement();
    }
}

std::unique_ptr<ASTNode> Parser::parseExpressionStatement() {
    auto expr = parseExpression(0); // Use Pratt parser for expressions
    expectToken(TokenType::T_SEMICOLON);
    return makeUnaryNode(ASTNodeType::EXPRESSION_STMT, std::move(expr));
}

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    // Consume the type token (we don't need to store it for now)
    nextToken(); // consume type

    if (currentToken.type != TokenType::T_IDENT) {
        unexpectedToken("identifier");
    }

    std::string varName = currentToken.value;
    nextToken(); // consume identifier

    auto decl = makeLeafNode(ASTNodeType::VAR_DECL, varName);

    // Check for initialization
    if (matchToken(TokenType::T_ASSIGN)) {
        auto initExpr = parseExpression(0); // Use Pratt parser
        decl->left = std::move(initExpr);
    }

    expectToken(TokenType::T_SEMICOLON);
    return decl;
}

std::unique_ptr<ASTNode> Parser::parseCoutStatement() {
    expectToken(TokenType::T_COUT);

    auto coutStmt = makeLeafNode(ASTNodeType::COUT_STMT);

    while (currentToken.type == TokenType::T_LSHIFT) {
        nextToken(); // consume <<
        auto expr = parseExpression(getOperatorPrecedence(TokenType::T_LSHIFT) + 1);
        coutStmt->children.push_back(std::move(expr));
    }

    expectToken(TokenType::T_SEMICOLON);
    return coutStmt;
}

std::unique_ptr<ASTNode> Parser::parseCinStatement() {
    expectToken(TokenType::T_CIN);

    auto cinStmt = makeLeafNode(ASTNodeType::CIN_STMT);

    while (currentToken.type == TokenType::T_RSHIFT) {
        nextToken(); // consume >>
        auto expr = parseExpression(getOperatorPrecedence(TokenType::T_RSHIFT) + 1);
        cinStmt->children.push_back(std::move(expr));
    }

    expectToken(TokenType::T_SEMICOLON);
    return cinStmt;
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    expectToken(TokenType::T_IF);
    expectToken(TokenType::T_LPAREN);

    auto condition = parseExpression(0);
    expectToken(TokenType::T_RPAREN);

    auto thenStmt = parseStatement();

    auto ifStmt = makeLeafNode(ASTNodeType::IF_STMT);
    ifStmt->children.push_back(std::move(condition));
    ifStmt->children.push_back(std::move(thenStmt));

    if (matchToken(TokenType::T_ELSE)) {
        auto elseStmt = parseStatement();
        ifStmt->children.push_back(std::move(elseStmt));
    }

    return ifStmt;
}

std::unique_ptr<ASTNode> Parser::parseWhileStatement() {
    expectToken(TokenType::T_WHILE);
    expectToken(TokenType::T_LPAREN);

    auto condition = parseExpression(0);
    expectToken(TokenType::T_RPAREN);

    auto body = parseStatement();

    auto whileStmt = makeLeafNode(ASTNodeType::WHILE_STMT);
    whileStmt->children.push_back(std::move(condition));
    whileStmt->children.push_back(std::move(body));

    return whileStmt;
}

std::unique_ptr<ASTNode> Parser::parseForStatement() {
    expectToken(TokenType::T_FOR);
    expectToken(TokenType::T_LPAREN);

    auto forStmt = makeLeafNode(ASTNodeType::FOR_STMT);

    // Initialization
    if (currentToken.type != TokenType::T_SEMICOLON) {
        forStmt->children.push_back(parseExpression(0));
    }
    expectToken(TokenType::T_SEMICOLON);

    // Condition
    if (currentToken.type != TokenType::T_SEMICOLON) {
        forStmt->children.push_back(parseExpression(0));
    }
    expectToken(TokenType::T_SEMICOLON);

    // Update
    if (currentToken.type != TokenType::T_RPAREN) {
        forStmt->children.push_back(parseExpression(0));
    }
    expectToken(TokenType::T_RPAREN);

    // Body
    forStmt->children.push_back(parseStatement());

    return forStmt;
}

std::unique_ptr<ASTNode> Parser::parseReturnStatement() {
    expectToken(TokenType::T_RETURN);

    auto returnStmt = makeLeafNode(ASTNodeType::RETURN_STMT);

    if (currentToken.type != TokenType::T_SEMICOLON) {
        returnStmt->left = parseExpression(0);
    }

    expectToken(TokenType::T_SEMICOLON);
    return returnStmt;
}

std::unique_ptr<ASTNode> Parser::parseCompoundStatement() {
    expectToken(TokenType::T_LBRACE);

    auto compound = makeLeafNode(ASTNodeType::COMPOUND_STMT);

    while (currentToken.type != TokenType::T_RBRACE && currentToken.type != TokenType::T_EOF) {
        auto stmt = parseStatement();
        if (stmt) {
            compound->children.push_back(std::move(stmt));
        }
    }

    expectToken(TokenType::T_RBRACE);
    return compound;
}

// Debug method to print AST
void Parser::printAST(const std::unique_ptr<ASTNode>& node, int indent) {
    if (!node) return;

    // Print indentation
    for (int i = 0; i < indent; i++) {
        std::cout << "  ";
    }

    // Print node information
    std::cout << astNodeTypeToString(node->type);

    if (!node->value.empty()) {
        std::cout << ": " << node->value;
    } else if (node->type == ASTNodeType::INTLIT) {
        std::cout << ": " << node->intValue;
    } else if (node->type == ASTNodeType::FLOATLIT) {
        std::cout << ": " << node->floatValue;
    }

    std::cout << std::endl;

    // Print children
    if (node->left) {
        printAST(node->left, indent + 1);
    }
    if (node->right) {
        printAST(node->right, indent + 1);
    }
    for (const auto& child : node->children) {
        printAST(child, indent + 1);
    }
}

std::string Parser::astNodeTypeToString(ASTNodeType type) {
    switch (type) {
        case ASTNodeType::INTLIT: return "INTLIT";
        case ASTNodeType::FLOATLIT: return "FLOATLIT";
        case ASTNodeType::STRINGLIT: return "STRINGLIT";
        case ASTNodeType::CHARLIT: return "CHARLIT";
        case ASTNodeType::IDENTIFIER: return "IDENTIFIER";
        case ASTNodeType::ADD: return "ADD";
        case ASTNodeType::SUBTRACT: return "SUBTRACT";
        case ASTNodeType::MULTIPLY: return "MULTIPLY";
        case ASTNodeType::DIVIDE: return "DIVIDE";
        case ASTNodeType::MODULO: return "MODULO";
        case ASTNodeType::ASSIGN: return "ASSIGN";
        case ASTNodeType::EQ: return "EQ";
        case ASTNodeType::NE: return "NE";
        case ASTNodeType::LT: return "LT";
        case ASTNodeType::GT: return "GT";
        case ASTNodeType::LE: return "LE";
        case ASTNodeType::GE: return "GE";
        case ASTNodeType::AND: return "AND";
        case ASTNodeType::OR: return "OR";
        case ASTNodeType::NOT: return "NOT";
        case ASTNodeType::NEGATE: return "NEGATE";
        case ASTNodeType::POSITIVE: return "POSITIVE";
        case ASTNodeType::EXPRESSION_STMT: return "EXPRESSION_STMT";
        case ASTNodeType::COUT_STMT: return "COUT_STMT";
        case ASTNodeType::CIN_STMT: return "CIN_STMT";
        case ASTNodeType::IF_STMT: return "IF_STMT";
        case ASTNodeType::WHILE_STMT: return "WHILE_STMT";
        case ASTNodeType::FOR_STMT: return "FOR_STMT";
        case ASTNodeType::RETURN_STMT: return "RETURN_STMT";
        case ASTNodeType::COMPOUND_STMT: return "COMPOUND_STMT";
        case ASTNodeType::VAR_DECL: return "VAR_DECL";
        case ASTNodeType::PROGRAM: return "PROGRAM";
        default: return "UNKNOWN";
    }
}
