#include "tokens.hpp"

std::unordered_map<std::string, TokenType> keywords;

void initializeKeywords() {
    keywords = {
        {"int", TokenType::T_INT},
        {"float", TokenType::T_FLOAT},
        {"char", TokenType::T_CHAR},
        {"double", TokenType::T_DOUBLE},
        {"bool", TokenType::T_BOOL},
        {"void", TokenType::T_VOID},
        {"if", TokenType::T_IF},
        {"else", TokenType::T_ELSE},
        {"while", TokenType::T_WHILE},
        {"for", TokenType::T_FOR},
        {"return", TokenType::T_RETURN},
        {"cout", TokenType::T_COUT},
        {"cin", TokenType::T_CIN},
        {"endl", TokenType::T_ENDL},
        {"true", TokenType::T_TRUE},
        {"false", TokenType::T_FALSE},
        {"const", TokenType::T_CONST},
        {"class", TokenType::T_CLASS},
        {"public", TokenType::T_PUBLIC},
        {"private", TokenType::T_PRIVATE},
        {"protected", TokenType::T_PROTECTED},
        {"namespace", TokenType::T_NAMESPACE},
        {"std", TokenType::T_STD},
        {"using", TokenType::T_USING},
        {"include", TokenType::T_INCLUDE}
    };
}

std::string getTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::T_EOF: return "EOF";
        case TokenType::T_INTLIT: return "INTEGER_LITERAL";
        case TokenType::T_FLOATLIT: return "FLOAT_LITERAL";
        case TokenType::T_STRINGLIT: return "STRING_LITERAL";
        case TokenType::T_CHARLIT: return "CHAR_LITERAL";
        case TokenType::T_IDENT: return "IDENTIFIER";
        case TokenType::T_INT: return "INT";
        case TokenType::T_FLOAT: return "FLOAT";
        case TokenType::T_CHAR: return "CHAR";
        case TokenType::T_DOUBLE: return "DOUBLE";
        case TokenType::T_BOOL: return "BOOL";
        case TokenType::T_VOID: return "VOID";
        case TokenType::T_IF: return "IF";
        case TokenType::T_ELSE: return "ELSE";
        case TokenType::T_WHILE: return "WHILE";
        case TokenType::T_FOR: return "FOR";
        case TokenType::T_RETURN: return "RETURN";
        case TokenType::T_COUT: return "COUT";
        case TokenType::T_CIN: return "CIN";
        case TokenType::T_ENDL: return "ENDL";
        case TokenType::T_TRUE: return "TRUE";
        case TokenType::T_FALSE: return "FALSE";
        case TokenType::T_CONST: return "CONST";
        case TokenType::T_CLASS: return "CLASS";
        case TokenType::T_PUBLIC: return "PUBLIC";
        case TokenType::T_PRIVATE: return "PRIVATE";
        case TokenType::T_PROTECTED: return "PROTECTED";
        case TokenType::T_NAMESPACE: return "NAMESPACE";
        case TokenType::T_STD: return "STD";
        case TokenType::T_USING: return "USING";
        case TokenType::T_INCLUDE: return "INCLUDE";
        case TokenType::T_PLUS: return "PLUS";
        case TokenType::T_MINUS: return "MINUS";
        case TokenType::T_STAR: return "STAR";
        case TokenType::T_SLASH: return "SLASH";
        case TokenType::T_PERCENT: return "PERCENT";
        case TokenType::T_ASSIGN: return "ASSIGN";
        case TokenType::T_EQ: return "EQ";
        case TokenType::T_NE: return "NE";
        case TokenType::T_LT: return "LT";
        case TokenType::T_GT: return "GT";
        case TokenType::T_LE: return "LE";
        case TokenType::T_GE: return "GE";
        case TokenType::T_AND: return "AND";
        case TokenType::T_OR: return "OR";
        case TokenType::T_NOT: return "NOT";
        case TokenType::T_BITAND: return "BITAND";
        case TokenType::T_BITOR: return "BITOR";
        case TokenType::T_BITXOR: return "BITXOR";
        case TokenType::T_BITNOT: return "BITNOT";
        case TokenType::T_LSHIFT: return "LSHIFT";
        case TokenType::T_RSHIFT: return "RSHIFT";
        case TokenType::T_INCREMENT: return "INCREMENT";
        case TokenType::T_DECREMENT: return "DECREMENT";
        case TokenType::T_PLUSEQ: return "PLUSEQ";
        case TokenType::T_MINUSEQ: return "MINUSEQ";
        case TokenType::T_STAREQ: return "STAREQ";
        case TokenType::T_SLASHEQ: return "SLASHEQ";
        case TokenType::T_ARROW: return "ARROW";
        case TokenType::T_SCOPE: return "SCOPE";
        case TokenType::T_SEMICOLON: return "SEMICOLON";
        case TokenType::T_COMMA: return "COMMA";
        case TokenType::T_LPAREN: return "LPAREN";
        case TokenType::T_RPAREN: return "RPAREN";
        case TokenType::T_LBRACE: return "LBRACE";
        case TokenType::T_RBRACE: return "RBRACE";
        case TokenType::T_LBRACKET: return "LBRACKET";
        case TokenType::T_RBRACKET: return "RBRACKET";
        case TokenType::T_COLON: return "COLON";
        case TokenType::T_DOT: return "DOT";
        case TokenType::T_QUESTION: return "QUESTION";
        case TokenType::T_HASH: return "HASH";
        case TokenType::T_NEWLINE: return "NEWLINE";
        case TokenType::T_WHITESPACE: return "WHITESPACE";
        case TokenType::T_COMMENT: return "COMMENT";
        case TokenType::T_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
