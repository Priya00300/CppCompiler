#include "scanner.hpp"
#include <iostream>
#include <iomanip>

void printToken(const Token& token) {
    std::cout << std::setw(15) << std::left << getTokenTypeName(token.type)
              << " | " << std::setw(20) << std::left << token.value
              << " | Line: " << std::setw(3) << token.line
              << " | Col: " << std::setw(3) << token.column << std::endl;
}

void printHeader() {
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "C++ LEXICAL SCANNER OUTPUT" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << std::setw(15) << std::left << "TOKEN TYPE"
              << " | " << std::setw(20) << std::left << "VALUE"
              << " | " << "POSITION" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        std::cerr << "Example: " << argv[0] << " test.cpp" << std::endl;
        return 1;
    }

    Scanner scanner;

    if (!scanner.initialize(argv[1])) {
        std::cerr << "Failed to initialize scanner with file: " << argv[1] << std::endl;
        return 1;
    }

    std::cout << "Scanning file: " << argv[1] << std::endl;
    printHeader();

    Token token;
    int tokenCount = 0;

    do {
        token = scanner.getNextToken();

        // Skip whitespace and newline tokens for cleaner output
        if (token.type != TokenType::T_WHITESPACE &&
            token.type != TokenType::T_NEWLINE) {
            printToken(token);
            tokenCount++;
        }

        // Handle errors
        if (token.type == TokenType::T_ERROR) {
            std::cerr << "Error: " << token.value << " at line "
                      << token.line << ", column " << token.column << std::endl;
        }

    } while (token.type != TokenType::T_EOF);

    std::cout << std::string(70, '-') << std::endl;
    std::cout << "Total tokens processed: " << tokenCount << std::endl;
    std::cout << "Scanning completed successfully!" << std::endl;

    return 0;
}
