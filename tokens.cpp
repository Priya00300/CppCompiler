#include "tokens.hpp"

// Global keyword map - this was declared as extern in tokens.hpp
std::unordered_map<std::string, TokenType> keywords;

// Function to initialize the keywords map
void initializeKeywords() {
    static bool initialized = false;
    if (initialized) return;  // Only initialize once

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

    initialized = true;
}

// Function to get a human-readable name for a token type
std::string getTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::T_EOF:
            return "EOF";

        // Literals
        case TokenType::T_INTLIT:
            return "INTEGER_LITERAL";
        case TokenType::T_FLOATLIT:
            return "FLOAT_LITERAL";
        case TokenType::T_STRINGLIT:
            return "STRING_LITERAL";
        case TokenType::T_CHARLIT:
            return "CHAR_LITERAL";
        case TokenType::T_IDENT:
            return "IDENTIFIER";

        // Data types
        case TokenType::T_INT:
            return "int";
        case TokenType::T_FLOAT:
            return "float";
        case TokenType::T_CHAR:
            return "char";
        case TokenType::T_DOUBLE:
            return "double";
        case TokenType::T_BOOL:
            return "bool";
        case TokenType::T_VOID:
            return "void";

        // Control flow keywords
        case TokenType::T_IF:
            return "if";
        case TokenType::T_ELSE:
            return "else";
        case TokenType::T_WHILE:
            return "while";
        case TokenType::T_FOR:
            return "for";
        case TokenType::T_RETURN:
            return "return";

        // I/O keywords
        case TokenType::T_COUT:
            return "cout";
        case TokenType::T_CIN:
            return "cin";
        case TokenType::T_ENDL:
            return "endl";

        // Boolean literals
        case TokenType::T_TRUE:
            return "true";
        case TokenType::T_FALSE:
            return "false";

        // Other keywords
        case TokenType::T_CONST:
            return "const";
        case TokenType::T_CLASS:
            return "class";
        case TokenType::T_PUBLIC:
            return "public";
        case TokenType::T_PRIVATE:
            return "private";
        case TokenType::T_PROTECTED:
            return "protected";
        case TokenType::T_NAMESPACE:
            return "namespace";
        case TokenType::T_STD:
            return "std";
        case TokenType::T_USING:
            return "using";
        case TokenType::T_INCLUDE:
            return "include";

        // Arithmetic operators
        case TokenType::T_PLUS:
            return "PLUS (+)";
        case TokenType::T_MINUS:
            return "MINUS (-)";
        case TokenType::T_STAR:
            return "MULTIPLY (*)";
        case TokenType::T_SLASH:
            return "DIVIDE (/)";
        case TokenType::T_PERCENT:
            return "MODULO (%)";

        // Assignment operators
        case TokenType::T_ASSIGN:
            return "ASSIGN (=)";
        case TokenType::T_PLUSEQ:
            return "PLUS_ASSIGN (+=)";
        case TokenType::T_MINUSEQ:
            return "MINUS_ASSIGN (-=)";
        case TokenType::T_STAREQ:
            return "MULTIPLY_ASSIGN (*=)";
        case TokenType::T_SLASHEQ:
            return "DIVIDE_ASSIGN (/=)";

        // Comparison operators
        case TokenType::T_EQ:
            return "EQUAL (==)";
        case TokenType::T_NE:
            return "NOT_EQUAL (!=)";
        case TokenType::T_LT:
            return "LESS_THAN (<)";
        case TokenType::T_GT:
            return "GREATER_THAN (>)";
        case TokenType::T_LE:
            return "LESS_EQUAL (<=)";
        case TokenType::T_GE:
            return "GREATER_EQUAL (>=)";

        // Logical operators
        case TokenType::T_AND:
            return "LOGICAL_AND (&&)";
        case TokenType::T_OR:
            return "LOGICAL_OR (||)";
        case TokenType::T_NOT:
            return "LOGICAL_NOT (!)";

        // Bitwise operators
        case TokenType::T_BITAND:
            return "BITWISE_AND (&)";
        case TokenType::T_BITOR:
            return "BITWISE_OR (|)";
        case TokenType::T_BITXOR:
            return "BITWISE_XOR (^)";
        case TokenType::T_BITNOT:
            return "BITWISE_NOT (~)";
        case TokenType::T_LSHIFT:
            return "LEFT_SHIFT (<<)";
        case TokenType::T_RSHIFT:
            return "RIGHT_SHIFT (>>)";

        // Increment/Decrement
        case TokenType::T_INCREMENT:
            return "INCREMENT (++)";
        case TokenType::T_DECREMENT:
            return "DECREMENT (--)";

        // Special operators
        case TokenType::T_ARROW:
            return "ARROW (->)";
        case TokenType::T_SCOPE:
            return "SCOPE (::)";

        // Delimiters
        case TokenType::T_SEMICOLON:
            return "SEMICOLON (;)";
        case TokenType::T_COMMA:
            return "COMMA (,)";
        case TokenType::T_LPAREN:
            return "LEFT_PAREN (()";
        case TokenType::T_RPAREN:
            return "RIGHT_PAREN ())";
        case TokenType::T_LBRACE:
            return "LEFT_BRACE ({)";
        case TokenType::T_RBRACE:
            return "RIGHT_BRACE (})";
        case TokenType::T_LBRACKET:
            return "LEFT_BRACKET ([)";
        case TokenType::T_RBRACKET:
            return "RIGHT_BRACKET (])";
        case TokenType::T_COLON:
            return "COLON (:)";
        case TokenType::T_DOT:
            return "DOT (.)";
        case TokenType::T_QUESTION:
            return "QUESTION (?)";

        // Preprocessor
        case TokenType::T_HASH:
            return "HASH (#)";

        // Special tokens
        case TokenType::T_NEWLINE:
            return "NEWLINE";
        case TokenType::T_WHITESPACE:
            return "WHITESPACE";
        case TokenType::T_COMMENT:
            return "COMMENT";

        // Error
        case TokenType::T_ERROR:
            return "ERROR";

        default:
            return "UNKNOWN_TOKEN";
    }
}
