#include "tokens.hpp"

// Keywords map
std::unordered_map<std::string, TokenType> keywords;

// Initialize keywords map
void initializeKeywords() {
    if (!keywords.empty()) return; // Already initialized

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

    // Modifiers
    keywords["const"] = TokenType::T_CONST;

    // Object-oriented
    keywords["class"] = TokenType::T_CLASS;
    keywords["public"] = TokenType::T_PUBLIC;
    keywords["private"] = TokenType::T_PRIVATE;
    keywords["protected"] = TokenType::T_PROTECTED;

    // Namespace
    keywords["namespace"] = TokenType::T_NAMESPACE;
    keywords["std"] = TokenType::T_STD;
    keywords["using"] = TokenType::T_USING;

    // Preprocessor
    keywords["include"] = TokenType::T_INCLUDE;
}

// Function to get token type name as string
std::string getTokenTypeName(TokenType type) {
    switch (type) {
        // End of file
        case TokenType::T_EOF: return "EOF";

        // Literals
        case TokenType::T_INTLIT: return "INTEGER_LITERAL";
        case TokenType::T_FLOATLIT: return "FLOAT_LITERAL";
        case TokenType::T_STRINGLIT: return "STRING_LITERAL";
        case TokenType::T_CHARLIT: return "CHARACTER_LITERAL";

        // Identifiers and keywords
        case TokenType::T_IDENT: return "IDENTIFIER";

        // Keywords - Data types
        case TokenType::T_INT: return "int";
        case TokenType::T_FLOAT: return "float";
        case TokenType::T_CHAR: return "char";
        case TokenType::T_DOUBLE: return "double";
        case TokenType::T_BOOL: return "bool";
        case TokenType::T_VOID: return "void";

        // Keywords - Control flow
        case TokenType::T_IF: return "if";
        case TokenType::T_ELSE: return "else";
        case TokenType::T_WHILE: return "while";
        case TokenType::T_FOR: return "for";
        case TokenType::T_RETURN: return "return";

        // Keywords - I/O
        case TokenType::T_COUT: return "cout";
        case TokenType::T_CIN: return "cin";
        case TokenType::T_ENDL: return "endl";

        // Keywords - Boolean
        case TokenType::T_TRUE: return "true";
        case TokenType::T_FALSE: return "false";

        // Keywords - Modifiers
        case TokenType::T_CONST: return "const";

        // Keywords - OOP
        case TokenType::T_CLASS: return "class";
        case TokenType::T_PUBLIC: return "public";
        case TokenType::T_PRIVATE: return "private";
        case TokenType::T_PROTECTED: return "protected";

        // Keywords - Namespace
        case TokenType::T_NAMESPACE: return "namespace";
        case TokenType::T_STD: return "std";
        case TokenType::T_USING: return "using";
        case TokenType::T_INCLUDE: return "include";

        // Operators - Arithmetic
        case TokenType::T_PLUS: return "+";
        case TokenType::T_MINUS: return "-";
        case TokenType::T_STAR: return "*";
        case TokenType::T_SLASH: return "/";
        case TokenType::T_PERCENT: return "%";

        // Operators - Assignment
        case TokenType::T_ASSIGN: return "=";
        case TokenType::T_PLUSEQ: return "+=";
        case TokenType::T_MINUSEQ: return "-=";
        case TokenType::T_STAREQ: return "*=";
        case TokenType::T_SLASHEQ: return "/=";

        // Operators - Comparison
        case TokenType::T_EQ: return "==";
        case TokenType::T_NE: return "!=";
        case TokenType::T_LT: return "<";
        case TokenType::T_GT: return ">";
        case TokenType::T_LE: return "<=";
        case TokenType::T_GE: return ">=";

        // Operators - Logical
        case TokenType::T_AND: return "&&";
        case TokenType::T_OR: return "||";
        case TokenType::T_NOT: return "!";

        // Operators - Bitwise
        case TokenType::T_BITAND: return "&";
        case TokenType::T_BITOR: return "|";
        case TokenType::T_BITXOR: return "^";
        case TokenType::T_BITNOT: return "~";
        case TokenType::T_LSHIFT: return "<<";
        case TokenType::T_RSHIFT: return ">>";

        // Operators - Increment/Decrement
        case TokenType::T_INCREMENT: return "++";
        case TokenType::T_DECREMENT: return "--";

        // Operators - Special
        case TokenType::T_ARROW: return "->";
        case TokenType::T_SCOPE: return "::";

        // Delimiters
        case TokenType::T_SEMICOLON: return ";";
        case TokenType::T_COMMA: return ",";
        case TokenType::T_LPAREN: return "(";
        case TokenType::T_RPAREN: return ")";
        case TokenType::T_LBRACE: return "{";
        case TokenType::T_RBRACE: return "}";
        case TokenType::T_LBRACKET: return "[";
        case TokenType::T_RBRACKET: return "]";
        case TokenType::T_COLON: return ":";
        case TokenType::T_DOT: return ".";
        case TokenType::T_QUESTION: return "?";

        // Preprocessor
        case TokenType::T_HASH: return "#";

        // Special
        case TokenType::T_NEWLINE: return "NEWLINE";
        case TokenType::T_WHITESPACE: return "WHITESPACE";
        case TokenType::T_COMMENT: return "COMMENT";

        // Error
        case TokenType::T_ERROR: return "ERROR";

        default: return "UNKNOWN";
    }
}
