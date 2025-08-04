#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <string>
#include <unordered_map>

// Token types enum
enum class TokenType {
    // End of file
    T_EOF = 0,

    // Literals
    T_INTLIT,       // Integer literal
    T_FLOATLIT,     // Float literal
    T_STRINGLIT,    // String literal
    T_CHARLIT,      // Character literal

    // Identifiers and keywords
    T_IDENT,        // Identifier

    // Keywords
    T_INT,          // int
    T_FLOAT,        // float
    T_CHAR,         // char
    T_DOUBLE,       // double
    T_BOOL,         // bool
    T_VOID,         // void
    T_IF,           // if
    T_ELSE,         // else
    T_WHILE,        // while
    T_FOR,          // for
    T_RETURN,       // return
    T_COUT,         // cout
    T_CIN,          // cin
    T_ENDL,         // endl
    T_TRUE,         // true
    T_FALSE,        // false
    T_CONST,        // const
    T_CLASS,        // class
    T_PUBLIC,       // public
    T_PRIVATE,      // private
    T_PROTECTED,    // protected
    T_NAMESPACE,    // namespace
    T_STD,          // std
    T_USING,        // using
    T_INCLUDE,      // include (for preprocessor)

    // Operators
    T_PLUS,         // +
    T_MINUS,        // -
    T_STAR,         // *
    T_SLASH,        // /
    T_PERCENT,      // %
    T_ASSIGN,       // =
    T_EQ,           // ==
    T_NE,           // !=
    T_LT,           // <
    T_GT,           // >
    T_LE,           // <=
    T_GE,           // >=
    T_AND,          // &&
    T_OR,           // ||
    T_NOT,          // !
    T_BITAND,       // &
    T_BITOR,        // |
    T_BITXOR,       // ^
    T_BITNOT,       // ~
    T_LSHIFT,       // <<
    T_RSHIFT,       // >>
    T_INCREMENT,    // ++
    T_DECREMENT,    // --
    T_PLUSEQ,       // +=
    T_MINUSEQ,      // -=
    T_STAREQ,       // *=
    T_SLASHEQ,      // /=
    T_ARROW,        // ->
    T_SCOPE,        // ::

    // Delimiters
    T_SEMICOLON,    // ;
    T_COMMA,        // ,
    T_LPAREN,       // (
    T_RPAREN,       // )
    T_LBRACE,       // {
    T_RBRACE,       // }
    T_LBRACKET,     // [
    T_RBRACKET,     // ]
    T_COLON,        // :
    T_DOT,          // .
    T_QUESTION,     // ?

    // Preprocessor
    T_HASH,         // #

    // Special
    T_NEWLINE,      // \n
    T_WHITESPACE,   // space, tab
    T_COMMENT,      // // or /* */

    // Error
    T_ERROR
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token() : type(TokenType::T_EOF), line(0), column(0) {}
    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

// Keywords map
extern std::unordered_map<std::string, TokenType> keywords;

// Function to initialize keywords
void initializeKeywords();

// Function to get token type name as string
std::string getTokenTypeName(TokenType type);

#endif // TOKENS_HPP
