#include "parser.hpp"
#include <iostream>
#include <iomanip>

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
            error("Invalid token type for AST conversion");
            return ASTNodeType::ADD; // This won't be reached due to error
    }
}

// Main parsing entry point
std::unique_ptr<ASTNode> Parser::parse() {
    return parseProgram();
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
    auto expr = parseExpression();
    expectToken(TokenType::T_SEMICOLON);
    return makeUnaryNode(ASTNodeType::EXPRESSION_STMT, std::move(expr));
}

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    nextToken(); // consume type token

    if (currentToken.type != TokenType::T_IDENT) {
        unexpectedToken("identifier");
    }

    std::string varName = currentToken.value;
    nextToken(); // consume identifier

    auto decl = makeLeafNode(ASTNodeType::VAR_DECL, varName);

    // Check for initialization
    if (matchToken(TokenType::T_ASSIGN)) {
        auto initExpr = parseExpression();
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
        auto expr = parseExpression();
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
        auto expr = parseExpression();
        cinStmt->children.push_back(std::move(expr));
    }

    expectToken(TokenType::T_SEMICOLON);
    return cinStmt;
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    expectToken(TokenType::T_IF);
    expectToken(TokenType::T_LPAREN);

    auto condition = parseExpression();
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

    auto condition = parseExpression();
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
        forStmt->children.push_back(parseExpression());
    }
    expectToken(TokenType::T_SEMICOLON);

    // Condition
    if (currentToken.type != TokenType::T_SEMICOLON) {
        forStmt->children.push_back(parseExpression());
    }
    expectToken(TokenType::T_SEMICOLON);

    // Update
    if (currentToken.type != TokenType::T_RPAREN) {
        forStmt->children.push_back(parseExpression());
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
        returnStmt->left = parseExpression();
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

// Expression parsing with operator precedence
std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseAssignmentExpression();
}

std::unique_ptr<ASTNode> Parser::parseAssignmentExpression() {
    auto left = parseLogicalOrExpression();

    if (currentToken.type == TokenType::T_ASSIGN) {
        TokenType op = currentToken.type;
        nextToken();
        auto right = parseAssignmentExpression();
        return makeBinaryNode(tokenToASTType(op), std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalOrExpression() {
    auto left = parseLogicalAndExpression();

    while (currentToken.type == TokenType::T_OR) {
        TokenType op = currentToken.type;
        nextToken();
        auto right = parseLogicalAndExpression();
        left = makeBinaryNode(tokenToASTType(op), std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAndExpression() {
    auto left = parseEqualityExpression();

    while (currentToken.type == TokenType::T_AND) {
        TokenType op = currentToken.type;
        nextToken();
        auto right = parseEqualityExpression();
        left = makeBinaryNode(tokenToASTType(op), std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseEqualityExpression() {
    auto left = parseRelationalExpression();

    while (currentToken.type == TokenType::T_EQ || currentToken.type == TokenType::T_NE) {
        TokenType op = currentToken.type;
        nextToken();
        auto right = parseRelationalExpression();
        left = makeBinaryNode(tokenToASTType(op), std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseRelationalExpression() {
    auto left = parseAdditiveExpression();

    while (currentToken.type == TokenType::T_LT || currentToken.type == TokenType::T_GT ||
           currentToken.type == TokenType::T_LE || currentToken.type == TokenType::T_GE) {
        TokenType op = currentToken.type;
        nextToken();
        auto right = parseAdditiveExpression();
        left = makeBinaryNode(tokenToASTType(op), std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseAdditiveExpression() {
    auto left = parseMultiplicativeExpression();

    while (currentToken.type == TokenType::T_PLUS || currentToken.type == TokenType::T_MINUS) {
        TokenType op = currentToken.type;
        nextToken();
        auto right = parseMultiplicativeExpression();
        left = makeBinaryNode(tokenToASTType(op), std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicativeExpression() {
    auto left = parseUnaryExpression();

    while (currentToken.type == TokenType::T_STAR || currentToken.type == TokenType::T_SLASH ||
           currentToken.type == TokenType::T_PERCENT) {
        TokenType op = currentToken.type;
        nextToken();
        auto right = parseUnaryExpression();
        left = makeBinaryNode(tokenToASTType(op), std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnaryExpression() {
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
            auto expr = parseExpression();
            expectToken(TokenType::T_RPAREN);
            return expr;
        }

        default:
            unexpectedToken("primary expression");
            return nullptr; // This won't be reached due to error
    }
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
