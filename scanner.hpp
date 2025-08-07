#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "tokens.hpp"
#include <fstream>
#include <string>
#include <vector>

class Scanner {
private:
    std::ifstream inputFile;
    std::string currentLine;
    size_t currentPos;
    int lineNumber;
    int columnNumber;
    char currentChar;
    bool isEOF;

public:
    Scanner();
    virtual ~Scanner();  // Make destructor virtual for proper inheritance

    // Initialize scanner with input file
    bool initialize(const std::string& filename);

    // Get the next token - VIRTUAL so StringScanner can override
    virtual Token getNextToken();

    // Peek at the next token without consuming it - VIRTUAL
    virtual Token peekToken();

    // Get current position info - VIRTUAL
    virtual int getCurrentLine() const { return lineNumber; }
    virtual int getCurrentColumn() const { return columnNumber; }

private:
    // Character handling
    void nextChar();
    char peekChar();
    void skipWhitespace();
    void skipComment();

    // Token scanning methods
    Token scanNumber();
    Token scanString();
    Token scanCharacter();
    Token scanIdentifier();
    Token scanOperator();

    // Utility methods
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlnum(char c);
    bool isHexDigit(char c);
    bool isOctalDigit(char c);

    // Error handling
    Token createErrorToken(const std::string& message);

    // Read next line from file
    bool readNextLine();
};

#endif // SCANNER_HPP
