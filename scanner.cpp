#include "scanner.hpp"
#include <iostream>
#include <sstream>
#include <cctype>

Scanner::Scanner() : currentPos(0), lineNumber(1), columnNumber(1),
                     currentChar('\0'), isEOF(false) {
    initializeKeywords();
}

Scanner::~Scanner() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
}

bool Scanner::initialize(const std::string& filename) {
    inputFile.open(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    // Read first line and set first character
    if (readNextLine()) {
        nextChar();
        return true;
    }

    isEOF = true;
    return true; // Empty file is valid
}

bool Scanner::readNextLine() {
    if (std::getline(inputFile, currentLine)) {
        currentLine += '\n'; // Add newline back
        currentPos = 0;
        lineNumber++;
        columnNumber = 1;
        return true;
    }
    return false;
}

void Scanner::nextChar() {
    if (isEOF) return;

    if (currentPos >= currentLine.length()) {
        if (!readNextLine()) {
            isEOF = true;
            currentChar = '\0';
            return;
        }
    }

    currentChar = currentLine[currentPos++];
    columnNumber++;
}

char Scanner::peekChar() {
    if (isEOF) return '\0';

    if (currentPos >= currentLine.length()) {
        return '\n';
    }

    return currentLine[currentPos];
}

void Scanner::skipWhitespace() {
    while (!isEOF && (currentChar == ' ' || currentChar == '\t' || currentChar == '\r')) {
        nextChar();
    }
}

void Scanner::skipComment() {
    if (currentChar == '/' && peekChar() == '/') {
        // Single line comment - skip to end of line
        while (!isEOF && currentChar != '\n') {
            nextChar();
        }
    } else if (currentChar == '/' && peekChar() == '*') {
        // Multi-line comment
        nextChar(); // skip '/'
        nextChar(); // skip '*'

        while (!isEOF) {
            if (currentChar == '*' && peekChar() == '/') {
                nextChar(); // skip '*'
                nextChar(); // skip '/'
                break;
            }
            nextChar();
        }
    }
}

Token Scanner::getNextToken() {
    while (!isEOF) {
        skipWhitespace();

        if (isEOF) break;

        int startLine = lineNumber;
        int startColumn = columnNumber;

        // Skip comments
        if (currentChar == '/' && (peekChar() == '/' || peekChar() == '*')) {
            skipComment();
            continue;
        }

        // Newlines
        if (currentChar == '\n') {
            nextChar();
            return Token(TokenType::T_NEWLINE, "\\n", startLine, startColumn);
        }

        // Numbers
        if (isDigit(currentChar)) {
            return scanNumber();
        }

        // Strings
        if (currentChar == '"') {
            return scanString();
        }

        // Characters
        if (currentChar == '\'') {
            return scanCharacter();
        }

        // Identifiers and keywords
        if (isAlpha(currentChar) || currentChar == '_') {
            return scanIdentifier();
        }

        // Operators and punctuation
        return scanOperator();
    }

    return Token(TokenType::T_EOF, "", lineNumber, columnNumber);
}

Token Scanner::scanNumber() {
    int startLine = lineNumber;
    int startColumn = columnNumber;
    std::string number;
    bool isFloat = false;

    // Handle decimal numbers
    while (!isEOF && (isDigit(currentChar) || currentChar == '.')) {
        if (currentChar == '.') {
            if (isFloat) break; // Second dot, stop here
            isFloat = true;
        }
        number += currentChar;
        nextChar();
    }

    // Handle scientific notation (e.g., 1.23e-4)
    if (!isEOF && (currentChar == 'e' || currentChar == 'E')) {
        number += currentChar;
        nextChar();

        if (!isEOF && (currentChar == '+' || currentChar == '-')) {
            number += currentChar;
            nextChar();
        }

        while (!isEOF && isDigit(currentChar)) {
            number += currentChar;
            nextChar();
        }

        isFloat = true;
    }

    // Handle suffix (f, F, l, L, etc.)
    if (!isEOF && (currentChar == 'f' || currentChar == 'F' ||
                   currentChar == 'l' || currentChar == 'L')) {
        if (currentChar == 'f' || currentChar == 'F') {
            isFloat = true;
        }
        number += currentChar;
        nextChar();
    }

    TokenType type = isFloat ? TokenType::T_FLOATLIT : TokenType::T_INTLIT;
    return Token(type, number, startLine, startColumn);
}

Token Scanner::scanString() {
    int startLine = lineNumber;
    int startColumn = columnNumber;
    std::string str;

    nextChar(); // skip opening quote

    while (!isEOF && currentChar != '"') {
        if (currentChar == '\\') {
            nextChar();
            if (isEOF) break;

            // Handle escape sequences
            switch (currentChar) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case 'r': str += '\r'; break;
                case '\\': str += '\\'; break;
                case '"': str += '"'; break;
                case '\'': str += '\''; break;
                case '0': str += '\0'; break;
                default:
                    str += '\\';
                    str += currentChar;
                    break;
            }
        } else {
            str += currentChar;
        }
        nextChar();
    }

    if (currentChar == '"') {
        nextChar(); // skip closing quote
    } else {
        return createErrorToken("Unterminated string literal");
    }

    return Token(TokenType::T_STRINGLIT, str, startLine, startColumn);
}

Token Scanner::scanCharacter() {
    int startLine = lineNumber;
    int startColumn = columnNumber;
    std::string ch;

    nextChar(); // skip opening quote

    if (!isEOF && currentChar != '\'') {
        if (currentChar == '\\') {
            nextChar();
            if (!isEOF) {
                // Handle escape sequences
                switch (currentChar) {
                    case 'n': ch = '\n'; break;
                    case 't': ch = '\t'; break;
                    case 'r': ch = '\r'; break;
                    case '\\': ch = '\\'; break;
                    case '"': ch = '"'; break;
                    case '\'': ch = '\''; break;
                    case '0': ch = '\0'; break;
                    default:
                        ch += '\\';
                        ch += currentChar;
                        break;
                }
                nextChar();
            }
        } else {
            ch += currentChar;
            nextChar();
        }
    }

    if (currentChar == '\'') {
        nextChar(); // skip closing quote
    } else {
        return createErrorToken("Unterminated character literal");
    }

    return Token(TokenType::T_CHARLIT, ch, startLine, startColumn);
}

Token Scanner::scanIdentifier() {
    int startLine = lineNumber;
    int startColumn = columnNumber;
    std::string identifier;

    while (!isEOF && (isAlnum(currentChar) || currentChar == '_')) {
        identifier += currentChar;
        nextChar();
    }

    // Check if it's a keyword
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return Token(it->second, identifier, startLine, startColumn);
    }

    return Token(TokenType::T_IDENT, identifier, startLine, startColumn);
}

Token Scanner::scanOperator() {
    int startLine = lineNumber;
    int startColumn = columnNumber;
    char firstChar = currentChar;
    nextChar();

    switch (firstChar) {
        case '+':
            if (currentChar == '+') {
                nextChar();
                return Token(TokenType::T_INCREMENT, "++", startLine, startColumn);
            } else if (currentChar == '=') {
                nextChar();
                return Token(TokenType::T_PLUSEQ, "+=", startLine, startColumn);
            }
            return Token(TokenType::T_PLUS, "+", startLine, startColumn);

        case '-':
            if (currentChar == '-') {
                nextChar();
                return Token(TokenType::T_DECREMENT, "--", startLine, startColumn);
            } else if (currentChar == '=') {
                nextChar();
                return Token(TokenType::T_MINUSEQ, "-=", startLine, startColumn);
            } else if (currentChar == '>') {
                nextChar();
                return Token(TokenType::T_ARROW, "->", startLine, startColumn);
            }
            return Token(TokenType::T_MINUS, "-", startLine, startColumn);

        case '*':
            if (currentChar == '=') {
                nextChar();
                return Token(TokenType::T_STAREQ, "*=", startLine, startColumn);
            }
            return Token(TokenType::T_STAR, "*", startLine, startColumn);

        case '/':
            if (currentChar == '=') {
                nextChar();
                return Token(TokenType::T_SLASHEQ, "/=", startLine, startColumn);
            }
            return Token(TokenType::T_SLASH, "/", startLine, startColumn);

        case '%':
            return Token(TokenType::T_PERCENT, "%", startLine, startColumn);

        case '=':
            if (currentChar == '=') {
                nextChar();
                return Token(TokenType::T_EQ, "==", startLine, startColumn);
            }
            return Token(TokenType::T_ASSIGN, "=", startLine, startColumn);

        case '!':
            if (currentChar == '=') {
                nextChar();
                return Token(TokenType::T_NE, "!=", startLine, startColumn);
            }
            return Token(TokenType::T_NOT, "!", startLine, startColumn);

        case '<':
            if (currentChar == '=') {
                nextChar();
                return Token(TokenType::T_LE, "<=", startLine, startColumn);
            } else if (currentChar == '<') {
                nextChar();
                return Token(TokenType::T_LSHIFT, "<<", startLine, startColumn);
            }
            return Token(TokenType::T_LT, "<", startLine, startColumn);

        case '>':
            if (currentChar == '=') {
                nextChar();
                return Token(TokenType::T_GE, ">=", startLine, startColumn);
            } else if (currentChar == '>') {
                nextChar();
                return Token(TokenType::T_RSHIFT, ">>", startLine, startColumn);
            }
            return Token(TokenType::T_GT, ">", startLine, startColumn);

        case '&':
            if (currentChar == '&') {
                nextChar();
                return Token(TokenType::T_AND, "&&", startLine, startColumn);
            }
            return Token(TokenType::T_BITAND, "&", startLine, startColumn);

        case '|':
            if (currentChar == '|') {
                nextChar();
                return Token(TokenType::T_OR, "||", startLine, startColumn);
            }
            return Token(TokenType::T_BITOR, "|", startLine, startColumn);

        case '^':
            return Token(TokenType::T_BITXOR, "^", startLine, startColumn);

        case '~':
            return Token(TokenType::T_BITNOT, "~", startLine, startColumn);

        case ':':
            if (currentChar == ':') {
                nextChar();
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
            return createErrorToken("Unexpected character: " + std::string(1, firstChar));
    }
}

Token Scanner::peekToken() {
    // Save current state
    size_t savedPos = currentPos;
    int savedLine = lineNumber;
    int savedCol = columnNumber;
    char savedChar = currentChar;
    bool savedEOF = isEOF;
    std::string savedCurrentLine = currentLine;

    // Get next token
    Token token = getNextToken();

    // Restore state
    currentPos = savedPos;
    lineNumber = savedLine;
    columnNumber = savedCol;
    currentChar = savedChar;
    isEOF = savedEOF;
    currentLine = savedCurrentLine;

    return token;
}

bool Scanner::isAlpha(char c) {
    return std::isalpha(c) || c == '_';
}

bool Scanner::isDigit(char c) {
    return std::isdigit(c);
}

bool Scanner::isAlnum(char c) {
    return std::isalnum(c);
}

bool Scanner::isHexDigit(char c) {
    return std::isxdigit(c);
}

bool Scanner::isOctalDigit(char c) {
    return c >= '0' && c <= '7';
}

Token Scanner::createErrorToken(const std::string& message) {
    return Token(TokenType::T_ERROR, message, lineNumber, columnNumber);
}
