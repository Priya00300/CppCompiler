#include "parser.hpp"
#include <iostream>
#include <iomanip>

Parser::Parser(Scanner* s) : scanner(s), ownedScanner(false) {
    nextToken();
}

Parser::Parser(const std::string& input) : scanner(new StringScanner(input)), ownedScanner(true) {
    nextToken();
}

Parser::~Parser() {
    if (ownedScanner && scanner) {
        delete scanner;
    }
}

void Parser::error(const std::string& message) {
    std::cerr << "Parser Error at line " << currentToken.line
              << ", column " << currentToken.column << ": " << message << std::endl;
    throw std::runtime_error("Parse error: " + message);
}

void Parser::nextToken() {
    do {
        currentToken = scanner->getNextToken();
    } while (currentToken.type == TokenType::T_WHITESPACE ||
             currentToken.type == TokenType::T_NEWLINE ||
             currentToken.type == TokenType::T_COMMENT);
}

void Parser::expectToken(TokenType expected) {
    if (currentToken.type != expected) {
        error("Expected " + getTokenTypeName(expected) + " but got " + getTokenTypeName(currentToken.type));
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

void Parser::synchronize() {
    nextToken();

    while (currentToken.type != TokenType::T_EOF) {
        if (currentToken.type == TokenType::T_SEMICOLON) return;

        switch (currentToken.type) {
            case TokenType::T_CLASS:
            case TokenType::T_FOR:
            case TokenType::T_IF:
            case TokenType::T_WHILE:
            case TokenType::T_RETURN:
                return;
            default:
                break;
        }

        nextToken();
    }
}

int Parser::getOperatorPrecedence(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::T_ASSIGN:
        case TokenType::T_PLUSEQ:
        case TokenType::T_MINUSEQ:
        case TokenType::T_STAREQ:
        case TokenType::T_SLASHEQ:
            return 2;
        case TokenType::T_OR:
            return 3;
        case TokenType::T_AND:
            return 5;
        case TokenType::T_EQ:
        case TokenType::T_NE:
            return 20;
        case TokenType::T_LT:
        case TokenType::T_GT:
        case TokenType::T_LE:
        case TokenType::T_GE:
            return 25;
        case TokenType::T_PLUS:
        case TokenType::T_MINUS:
            return 30;
        case TokenType::T_LSHIFT:
        case TokenType::T_RSHIFT:
            return 35;
        case TokenType::T_STAR:
        case TokenType::T_SLASH:
        case TokenType::T_PERCENT:
            return 40;
        case TokenType::T_INCREMENT:
        case TokenType::T_DECREMENT:
            return 50;
        default:
            return 0;
    }
}

bool Parser::isRightAssociative(TokenType tokenType) {
    return tokenType == TokenType::T_ASSIGN ||
           tokenType == TokenType::T_PLUSEQ ||
           tokenType == TokenType::T_MINUSEQ ||
           tokenType == TokenType::T_STAREQ ||
           tokenType == TokenType::T_SLASHEQ;
}

ASTNodeType Parser::tokenToASTNode(TokenType tokenType) {
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
            error("Invalid token type for AST conversion");
            return ASTNodeType::ADD;
    }
}

// Main parsing entry point
std::unique_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

// Pratt parser implementation for expressions
std::unique_ptr<ASTNode> Parser::parseExpression(int minPrecedence) {
    // Parse the left-hand side (could be primary or unary expression)
    auto left = parseUnary();

    // While we have operators with sufficient precedence
    while (getOperatorPrecedence(currentToken.type) >= minPrecedence &&
           getOperatorPrecedence(currentToken.type) > 0) {

        TokenType operatorToken = currentToken.type;
        int operatorPrec = getOperatorPrecedence(operatorToken);

        nextToken(); // consume the operator

        // For right-associative operators, use the same precedence
        // For left-associative operators, use precedence + 1
        int nextMinPrec = isRightAssociative(operatorToken) ? operatorPrec : operatorPrec + 1;

        // Parse the right-hand side with appropriate precedence
        auto right = parseExpression(nextMinPrec);

        // Create binary operator node
        auto binaryNode = std::make_unique<ASTNode>(tokenToASTNode(operatorToken));
        binaryNode->left = std::move(left);
        binaryNode->right = std::move(right);
        left = std::move(binaryNode);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnary() {
    // Handle unary operators
    if (currentToken.type == TokenType::T_MINUS) {
        nextToken();
        auto expr = parseUnary();
        auto unaryNode = std::make_unique<ASTNode>(ASTNodeType::NEGATE);
        unaryNode->left = std::move(expr);
        return unaryNode;
    }

    if (currentToken.type == TokenType::T_PLUS) {
        nextToken();
        auto expr = parseUnary();
        auto unaryNode = std::make_unique<ASTNode>(ASTNodeType::POSITIVE);
        unaryNode->left = std::move(expr);
        return unaryNode;
    }

    if (currentToken.type == TokenType::T_NOT) {
        nextToken();
        auto expr = parseUnary();
        auto unaryNode = std::make_unique<ASTNode>(ASTNodeType::NOT);
        unaryNode->left = std::move(expr);
        return unaryNode;
    }

    return parsePrimary();
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    switch (currentToken.type) {
        case TokenType::T_INTLIT: {
            auto node = std::make_unique<ASTNode>(ASTNodeType::INTLIT);
            node->intValue = std::stoi(currentToken.value);
            node->value = currentToken.value;
            nextToken();
            return node;
        }

        case TokenType::T_FLOATLIT: {
            auto node = std::make_unique<ASTNode>(ASTNodeType::FLOATLIT);
            node->floatValue = std::stof(currentToken.value);
            node->value = currentToken.value;
            nextToken();
            return node;
        }

        case TokenType::T_STRINGLIT: {
            auto node = std::make_unique<ASTNode>(ASTNodeType::STRINGLIT);
            node->value = currentToken.value;
            nextToken();
            return node;
        }

        case TokenType::T_CHARLIT: {
            auto node = std::make_unique<ASTNode>(ASTNodeType::CHARLIT);
            node->value = currentToken.value;
            nextToken();
            return node;
        }

        case TokenType::T_IDENT: {
            auto node = std::make_unique<ASTNode>(ASTNodeType::IDENTIFIER);
            node->value = currentToken.value;
            nextToken();
            return node;
        }

        case TokenType::T_TRUE: {
            auto node = std::make_unique<ASTNode>(ASTNodeType::BOOLLIT);
            node->boolValue = true;
            node->value = "true";
            nextToken();
            return node;
        }

        case TokenType::T_FALSE: {
            auto node = std::make_unique<ASTNode>(ASTNodeType::BOOLLIT);
            node->boolValue = false;
            node->value = "false";
            nextToken();
            return node;
        }

        case TokenType::T_LPAREN: {
            nextToken(); // consume '('
            auto expr = parseExpression(0); // Start with minimum precedence
            expectToken(TokenType::T_RPAREN);
            return expr;
        }

        default:
            error("Expected primary expression");
            return nullptr;
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
    auto program = std::make_unique<ASTNode>(ASTNodeType::PROGRAM);

    // Skip initial newlines
    while (currentToken.type == TokenType::T_NEWLINE) {
        nextToken();
    }

    // Parse statements
    while (currentToken.type != TokenType::T_EOF) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->children.push_back(std::move(stmt));
            }
        } catch (const std::runtime_error& e) {
            synchronize();
        }

        // Skip newlines between statements
        while (currentToken.type == TokenType::T_NEWLINE) {
            nextToken();
        }
    }

    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    // Skip newlines
    while (currentToken.type == TokenType::T_NEWLINE) {
        nextToken();
    }

    switch (currentToken.type) {
        case TokenType::T_INT:
        case TokenType::T_FLOAT:
        case TokenType::T_CHAR:
        case TokenType::T_DOUBLE:
        case TokenType::T_BOOL:
            return parseVariableDeclaration();

        case TokenType::T_IF:
            return parseIfStatement();

        case TokenType::T_WHILE:
            return parseWhileStatement();

        case TokenType::T_FOR:
            return parseForStatement();

        case TokenType::T_RETURN:
            return parseReturnStatement();

        case TokenType::T_COUT:
            return parseCoutStatement();

        case TokenType::T_CIN:
            return parseCinStatement();

        case TokenType::T_LBRACE:
            return parseCompoundStatement();

        case TokenType::T_SEMICOLON:
            // Empty statement
            nextToken();
            return nullptr;

        default:
            return parseExpressionStatement();
    }
}

std::unique_ptr<ASTNode> Parser::parseVariableDeclaration() {
    // Parse: int x; or int x = expression;
    auto node = std::make_unique<ASTNode>(ASTNodeType::VAR_DECL);

    // Skip the type token (int, float, etc.)
    nextToken();

    // Get variable name
    if (currentToken.type != TokenType::T_IDENT) {
        error("Expected variable name in declaration");
        return nullptr;
    }

    node->value = currentToken.value;
    nextToken();

    // Check for initialization
    if (currentToken.type == TokenType::T_ASSIGN) {
        nextToken();
        node->left = parseExpression();
    }

    expectToken(TokenType::T_SEMICOLON);
    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpressionStatement() {
    auto node = std::make_unique<ASTNode>(ASTNodeType::EXPRESSION_STMT);
    node->left = parseExpression();
    expectToken(TokenType::T_SEMICOLON);
    return node;
}

std::unique_ptr<ASTNode> Parser::parseCompoundStatement() {
    // Parse: { statement1; statement2; ... }
    auto node = std::make_unique<ASTNode>(ASTNodeType::COMPOUND_STMT);

    expectToken(TokenType::T_LBRACE);

    while (currentToken.type != TokenType::T_RBRACE && currentToken.type != TokenType::T_EOF) {
        auto stmt = parseStatement();
        if (stmt) {
            node->children.push_back(std::move(stmt));
        }
    }

    expectToken(TokenType::T_RBRACE);
    return node;
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    // Parse: if (condition) statement [else statement]
    auto node = std::make_unique<ASTNode>(ASTNodeType::IF_STMT);

    nextToken(); // Skip 'if'
    expectToken(TokenType::T_LPAREN);

    node->condition = parseExpression();

    expectToken(TokenType::T_RPAREN);

    node->left = parseStatement(); // Then statement

    if (currentToken.type == TokenType::T_ELSE) {
        nextToken();
        node->right = parseStatement(); // Else statement
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseWhileStatement() {
    // Parse: while (condition) statement
    auto node = std::make_unique<ASTNode>(ASTNodeType::WHILE_STMT);

    nextToken(); // Skip 'while'
    expectToken(TokenType::T_LPAREN);

    node->condition = parseExpression();

    expectToken(TokenType::T_RPAREN);

    node->left = parseStatement();

    return node;
}

std::unique_ptr<ASTNode> Parser::parseForStatement() {
    // Parse: for (init; condition; update) statement
    auto node = std::make_unique<ASTNode>(ASTNodeType::FOR_STMT);

    nextToken(); // Skip 'for'
    expectToken(TokenType::T_LPAREN);

    // Init statement (can be empty)
    if (currentToken.type != TokenType::T_SEMICOLON) {
        if (currentToken.type == TokenType::T_INT || currentToken.type == TokenType::T_FLOAT ||
            currentToken.type == TokenType::T_CHAR || currentToken.type == TokenType::T_DOUBLE ||
            currentToken.type == TokenType::T_BOOL) {
            node->children.push_back(parseVariableDeclaration());
        } else {
            auto expr = parseExpression();
            expectToken(TokenType::T_SEMICOLON);
            node->children.push_back(std::move(expr));
        }
    } else {
        nextToken(); // Skip semicolon
    }

    // Condition (can be empty)
    if (currentToken.type != TokenType::T_SEMICOLON) {
        node->condition = parseExpression();
    }
    expectToken(TokenType::T_SEMICOLON);

    // Update expression (can be empty)
    if (currentToken.type != TokenType::T_RPAREN) {
        node->children.push_back(parseExpression());
    }

    expectToken(TokenType::T_RPAREN);

    // Body
    node->left = parseStatement();

    return node;
}

std::unique_ptr<ASTNode> Parser::parseReturnStatement() {
    // Parse: return [expression];
    auto node = std::make_unique<ASTNode>(ASTNodeType::RETURN_STMT);

    nextToken(); // Skip 'return'

    if (currentToken.type != TokenType::T_SEMICOLON) {
        node->left = parseExpression();
    }

    expectToken(TokenType::T_SEMICOLON);
    return node;
}

std::unique_ptr<ASTNode> Parser::parseCoutStatement() {
    // Parse: cout << expression;
    auto node = std::make_unique<ASTNode>(ASTNodeType::COUT_STMT);

    nextToken(); // Skip 'cout'

    while (currentToken.type == TokenType::T_LSHIFT) {
        nextToken(); // consume <<
        auto expr = parseExpression(getOperatorPrecedence(TokenType::T_LSHIFT) + 1);
        node->children.push_back(std::move(expr));
    }

    expectToken(TokenType::T_SEMICOLON);
    return node;
}

std::unique_ptr<ASTNode> Parser::parseCinStatement() {
    // Parse: cin >> variable;
    auto node = std::make_unique<ASTNode>(ASTNodeType::CIN_STMT);

    nextToken(); // Skip 'cin'

    while (currentToken.type == TokenType::T_RSHIFT) {
        nextToken(); // consume >>
        auto expr = parseExpression(getOperatorPrecedence(TokenType::T_RSHIFT) + 1);
        node->children.push_back(std::move(expr));
    }

    expectToken(TokenType::T_SEMICOLON);
    return node;
}

// AST printing with statement support
void Parser::printAST(const std::unique_ptr<ASTNode>& node, int depth) {
    if (!node) return;

    std::string indent(depth * 2, ' ');
    std::cout << indent << astNodeTypeToString(node->type);

    switch (node->type) {
        case ASTNodeType::INTLIT:
            std::cout << " (" << node->intValue << ")";
            break;
        case ASTNodeType::FLOATLIT:
            std::cout << " (" << node->floatValue << ")";
            break;
        case ASTNodeType::BOOLLIT:
            std::cout << " (" << (node->boolValue ? "true" : "false") << ")";
            break;
        case ASTNodeType::IDENTIFIER:
        case ASTNodeType::VAR_DECL:
        case ASTNodeType::STRINGLIT:
        case ASTNodeType::CHARLIT:
            if (!node->value.empty()) {
                std::cout << " (" << node->value << ")";
            }
            break;
        default:
            break;
    }

    std::cout << std::endl;

    // Print condition for conditional statements
    if (node->condition) {
        std::cout << indent << "  Condition:" << std::endl;
        printAST(node->condition, depth + 2);
    }

    // Print left child
    if (node->left) {
        std::cout << indent << "  Left:" << std::endl;
        printAST(node->left, depth + 2);
    }

    // Print right child
    if (node->right) {
        std::cout << indent << "  Right:" << std::endl;
        printAST(node->right, depth + 2);
    }

    // Print children for compound statements
    if (!node->children.empty()) {
        std::cout << indent << "  Children:" << std::endl;
        for (const auto& child : node->children) {
            printAST(child, depth + 2);
        }
    }
}

std::string Parser::astNodeTypeToString(ASTNodeType type) {
    switch (type) {
        case ASTNodeType::INTLIT: return "INTLIT";
        case ASTNodeType::FLOATLIT: return "FLOATLIT";
        case ASTNodeType::STRINGLIT: return "STRINGLIT";
        case ASTNodeType::CHARLIT: return "CHARLIT";
        case ASTNodeType::BOOLLIT: return "BOOLLIT";
        case ASTNodeType::IDENTIFIER: return "IDENTIFIER";
        case ASTNodeType::ADD: return "ADD";
        case ASTNodeType::SUBTRACT: return "SUBTRACT";
        case ASTNodeType::MULTIPLY: return "MULTIPLY";
        case ASTNodeType::DIVIDE: return "DIVIDE";
        case ASTNodeType::MODULO: return "MODULO";
        case ASTNodeType::EQ: return "EQUAL";
        case ASTNodeType::NE: return "NOT_EQUAL";
        case ASTNodeType::LT: return "LESS_THAN";
        case ASTNodeType::GT: return "GREATER_THAN";
        case ASTNodeType::LE: return "LESS_EQUAL";
        case ASTNodeType::GE: return "GREATER_EQUAL";
        case ASTNodeType::AND: return "LOGICAL_AND";
        case ASTNodeType::OR: return "LOGICAL_OR";
        case ASTNodeType::NOT: return "LOGICAL_NOT";
        case ASTNodeType::NEGATE: return "NEGATE";
        case ASTNodeType::POSITIVE: return "POSITIVE";
        case ASTNodeType::ASSIGN: return "ASSIGN";
        case ASTNodeType::VAR_DECL: return "VAR_DECLARATION";
        case ASTNodeType::EXPRESSION_STMT: return "EXPRESSION_STMT";
        case ASTNodeType::COMPOUND_STMT: return "COMPOUND_STMT";
        case ASTNodeType::IF_STMT: return "IF_STATEMENT";
        case ASTNodeType::WHILE_STMT: return "WHILE_STATEMENT";
        case ASTNodeType::FOR_STMT: return "FOR_STATEMENT";
        case ASTNodeType::RETURN_STMT: return "RETURN_STATEMENT";
        case ASTNodeType::COUT_STMT: return "COUT_STATEMENT";
        case ASTNodeType::CIN_STMT: return "CIN_STATEMENT";
        case ASTNodeType::PROGRAM: return "PROGRAM";
        case ASTNodeType::BLOCK: return "BLOCK";
        default: return "UNKNOWN";
    }
}

// StringScanner implementation
StringScanner::StringScanner(const std::string& text) : input(text), position(0), line(1), column(1) {
    initializeKeywords();
}

void StringScanner::initializeKeywords() {
    // Clear the map first
    keywords.clear();

    // Data types
    keywords["int"] = TokenType::T_INT;
    keywords["float"] = TokenType::T_FLOAT;
    keywords["char"] = TokenType::T_CHAR;
    keywords["double"] = TokenType::T_DOUBLE;
    keywords["bool"] = TokenType::T_BOOL;
    keywords["void"] = TokenType::T_VOID;

    // Control flow
    keywords["if"] = TokenType::T_IF;
    keywords["else"] = TokenType::T_ELSE;
    keywords["while"] = TokenType::T_WHILE;
    keywords["for"] = TokenType::T_FOR;
    keywords["return"] = TokenType::T_RETURN;

    // I/O
    keywords["cout"] = TokenType::T_COUT;
    keywords["cin"] = TokenType::T_CIN;
    keywords["endl"] = TokenType::T_ENDL;

    // Boolean literals
    keywords["true"] = TokenType::T_TRUE;
    keywords["false"] = TokenType::T_FALSE;

    // Other keywords
    keywords["const"] = TokenType::T_CONST;
    keywords["class"] = TokenType::T_CLASS;
    keywords["public"] = TokenType::T_PUBLIC;
    keywords["private"] = TokenType::T_PRIVATE;
    keywords["protected"] = TokenType::T_PROTECTED;
    keywords["namespace"] = TokenType::T_NAMESPACE;
    keywords["std"] = TokenType::T_STD;
    keywords["using"] = TokenType::T_USING;
    keywords["include"] = TokenType::T_INCLUDE;
}

Token StringScanner::getNextToken() {
    while (!isAtEnd()) {
        skipWhitespace();

        if (isAtEnd()) break;

        int startLine = line;
        int startColumn = column;

        // Skip comments
        if (currentChar() == '/' && peekChar() == '/') {
            // Single line comment - skip to end
            while (!isAtEnd() && currentChar() != '\n') {
                advance();
            }
            continue;
        } else if (currentChar() == '/' && peekChar() == '*') {
            // Multi-line comment
            advance(); // skip '/'
            advance(); // skip '*'

            while (!isAtEnd()) {
                if (currentChar() == '*' && peekChar() == '/') {
                    advance(); // skip '*'
                    advance(); // skip '/'
                    break;
                }
                if (currentChar() == '\n') {
                    line++;
                    column = 1;
                } else {
                    column++;
                }
                position++;
            }
            continue;
        }

        // Newlines
        if (currentChar() == '\n') {
            advance();
            line++;
            column = 1;
            return Token(TokenType::T_NEWLINE, "\\n", startLine, startColumn);
        }

        // Numbers
        if (std::isdigit(currentChar())) {
            return scanNumber();
        }

        // Strings
        if (currentChar() == '"') {
            return scanString();
        }

        // Characters
        if (currentChar() == '\'') {
            return scanCharacter();
        }

        // Identifiers and keywords
        if (std::isalpha(currentChar()) || currentChar() == '_') {
            return scanIdentifier();
        }

        // Operators and punctuation
        return scanOperator();
    }

    return Token(TokenType::T_EOF, "", line, column);
}

Token StringScanner::peekToken() {
    // Save current state
    size_t savedPos = position;
    int savedLine = line;
    int savedCol = column;

    // Get next token
    Token token = getNextToken();

    // Restore state
    position = savedPos;
    line = savedLine;
    column = savedCol;

    return token;
}

void StringScanner::skipWhitespace() {
    while (!isAtEnd() && (currentChar() == ' ' || currentChar() == '\t' || currentChar() == '\r')) {
        advance();
    }
}

Token StringScanner::scanNumber() {
    int startLine = line;
    int startColumn = column;
    std::string number;
    bool isFloat = false;

    // Handle decimal numbers
    while (!isAtEnd() && (std::isdigit(currentChar()) || currentChar() == '.')) {
        if (currentChar() == '.') {
            if (isFloat) break; // Second dot, stop here
            isFloat = true;
        }
        number += currentChar();
        advance();
    }

    // Handle scientific notation (e.g., 1.23e-4)
    if (!isAtEnd() && (currentChar() == 'e' || currentChar() == 'E')) {
        number += currentChar();
        advance();

        if (!isAtEnd() && (currentChar() == '+' || currentChar() == '-')) {
            number += currentChar();
            advance();
        }

        while (!isAtEnd() && std::isdigit(currentChar())) {
            number += currentChar();
            advance();
        }

        isFloat = true;
    }

    // Handle suffix (f, F, l, L, etc.)
    if (!isAtEnd() && (currentChar() == 'f' || currentChar() == 'F' ||
                       currentChar() == 'l' || currentChar() == 'L')) {
        if (currentChar() == 'f' || currentChar() == 'F') {
            isFloat = true;
        }
        number += currentChar();
        advance();
    }

    TokenType type = isFloat ? TokenType::T_FLOATLIT : TokenType::T_INTLIT;
    return Token(type, number, startLine, startColumn);
}

Token StringScanner::scanString() {
    int startLine = line;
    int startColumn = column;
    std::string str;

    advance(); // skip opening quote

    while (!isAtEnd() && currentChar() != '"') {
        if (currentChar() == '\\') {
            advance();
            if (isAtEnd()) break;

            // Handle escape sequences
            switch (currentChar()) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case 'r': str += '\r'; break;
                case '\\': str += '\\'; break;
                case '"': str += '"'; break;
                case '\'': str += '\''; break;
                case '0': str += '\0'; break;
                default:
                    str += '\\';
                    str += currentChar();
                    break;
            }
        } else {
            str += currentChar();
        }
        advance();
    }

    if (currentChar() == '"') {
        advance(); // skip closing quote
    } else {
        return Token(TokenType::T_ERROR, "Unterminated string literal", line, column);
    }

    return Token(TokenType::T_STRINGLIT, str, startLine, startColumn);
}

Token StringScanner::scanCharacter() {
    int startLine = line;
    int startColumn = column;
    std::string ch;

    advance(); // skip opening quote

    if (!isAtEnd() && currentChar() != '\'') {
        if (currentChar() == '\\') {
            advance();
            if (!isAtEnd()) {
                // Handle escape sequences
                switch (currentChar()) {
                    case 'n': ch = '\n'; break;
                    case 't': ch = '\t'; break;
                    case 'r': ch = '\r'; break;
                    case '\\': ch = '\\'; break;
                    case '"': ch = '"'; break;
                    case '\'': ch = '\''; break;
                    case '0': ch = '\0'; break;
                    default:
                        ch += '\\';
                        ch += currentChar();
                        break;
                }
                advance();
            }
        } else {
            ch += currentChar();
            advance();
        }
    }

    if (currentChar() == '\'') {
        advance(); // skip closing quote
    } else {
        return Token(TokenType::T_ERROR, "Unterminated character literal", line, column);
    }

    return Token(TokenType::T_CHARLIT, ch, startLine, startColumn);
}

Token StringScanner::scanIdentifier() {
    int startLine = line;
    int startColumn = column;
    std::string identifier;

    while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '_')) {
        identifier += currentChar();
        advance();
    }

    // Check if it's a keyword
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return Token(it->second, identifier, startLine, startColumn);
    }

    return Token(TokenType::T_IDENT, identifier, startLine, startColumn);
}

Token StringScanner::scanOperator() {
    int startLine = line;
    int startColumn = column;
    char firstChar = currentChar();
    advance();

    switch (firstChar) {
        case '+':
            if (currentChar() == '+') {
                advance();
                return Token(TokenType::T_INCREMENT, "++", startLine, startColumn);
            } else if (currentChar() == '=') {
                advance();
                return Token(TokenType::T_PLUSEQ, "+=", startLine, startColumn);
            }
            return Token(TokenType::T_PLUS, "+", startLine, startColumn);

        case '-':
            if (currentChar() == '-') {
                advance();
                return Token(TokenType::T_DECREMENT, "--", startLine, startColumn);
            } else if (currentChar() == '=') {
                advance();
                return Token(TokenType::T_MINUSEQ, "-=", startLine, startColumn);
            } else if (currentChar() == '>') {
                advance();
                return Token(TokenType::T_ARROW, "->", startLine, startColumn);
            }
            return Token(TokenType::T_MINUS, "-", startLine, startColumn);

        case '*':
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::T_STAREQ, "*=", startLine, startColumn);
            }
            return Token(TokenType::T_STAR, "*", startLine, startColumn);

        case '/':
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::T_SLASHEQ, "/=", startLine, startColumn);
            }
            return Token(TokenType::T_SLASH, "/", startLine, startColumn);

        case '%':
            return Token(TokenType::T_PERCENT, "%", startLine, startColumn);

        case '=':
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::T_EQ, "==", startLine, startColumn);
            }
            return Token(TokenType::T_ASSIGN, "=", startLine, startColumn);

        case '!':
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::T_NE, "!=", startLine, startColumn);
            }
            return Token(TokenType::T_NOT, "!", startLine, startColumn);

        case '<':
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::T_LE, "<=", startLine, startColumn);
            } else if (currentChar() == '<') {
                advance();
                return Token(TokenType::T_LSHIFT, "<<", startLine, startColumn);
            }
            return Token(TokenType::T_LT, "<", startLine, startColumn);

        case '>':
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::T_GE, ">=", startLine, startColumn);
            } else if (currentChar() == '>') {
                advance();
                return Token(TokenType::T_RSHIFT, ">>", startLine, startColumn);
            }
            return Token(TokenType::T_GT, ">", startLine, startColumn);

        case '&':
            if (currentChar() == '&') {
                advance();
                return Token(TokenType::T_AND, "&&", startLine, startColumn);
            }
            return Token(TokenType::T_BITAND, "&", startLine, startColumn);

        case '|':
            if (currentChar() == '|') {
                advance();
                return Token(TokenType::T_OR, "||", startLine, startColumn);
            }
            return Token(TokenType::T_BITOR, "|", startLine, startColumn);

        case '^':
            return Token(TokenType::T_BITXOR, "^", startLine, startColumn);

        case '~':
            return Token(TokenType::T_BITNOT, "~", startLine, startColumn);

        case ':':
            if (currentChar() == ':') {
                advance();
                return Token(TokenType::T_SCOPE, "::", startLine, startColumn);
            }
            return Token(TokenType::T_COLON, ":", startLine, startColumn);

        case ';':
            return Token(TokenType::T_SEMICOLON, ";", startLine, startColumn);

        case ',':
            return Token(TokenType::T_COMMA, ",", startLine, startColumn);

        case '(':
            return Token(TokenType::T_LPAREN, "(", startLine, startColumn);

        case ')':
            return Token(TokenType::T_RPAREN, ")", startLine, startColumn);

        case '{':
            return Token(TokenType::T_LBRACE, "{", startLine, startColumn);

        case '}':
            return Token(TokenType::T_RBRACE, "}", startLine, startColumn);

        case '[':
            return Token(TokenType::T_LBRACKET, "[", startLine, startColumn);

        case ']':
            return Token(TokenType::T_RBRACKET, "]", startLine, startColumn);

        case '.':
            return Token(TokenType::T_DOT, ".", startLine, startColumn);

        case '?':
            return Token(TokenType::T_QUESTION, "?", startLine, startColumn);

        case '#':
            return Token(TokenType::T_HASH, "#", startLine, startColumn);

        default:
            return Token(TokenType::T_ERROR, "Unexpected character: " + std::string(1, firstChar), line, column);
    }
}
