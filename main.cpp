#include <iostream>
#include <fstream>
#include <memory>
#include "tokens.hpp"
#include "scanner.hpp"
#include "parser.hpp"

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
            std::cerr << "Example: " << argv[0] << " test.cpp" << std::endl;
            return 1;
        }

        // Initialize scanner with input file
        Scanner scanner;
        if (!scanner.initialize(argv[1])) {
            std::cerr << "Error: Could not open file '" << argv[1] << "'" << std::endl;
            return 1;
        }

        std::cout << "=== Scanning and Parsing: " << argv[1] << " ===" << std::endl;

        // Initialize parser with scanner
        Parser parser(&scanner);

        std::cout << "Parsing..." << std::endl;

        // Parse the program and build AST
        auto ast = parser.parse();

        std::cout << "Parsing completed successfully!" << std::endl;
        std::cout << "\n=== Abstract Syntax Tree ===" << std::endl;

        // Print the AST
        parser.printAST(ast);

        std::cout << "\n=== Parsing Summary ===" << std::endl;
        std::cout << "✓ Lexical analysis completed" << std::endl;
        std::cout << "✓ Syntax analysis completed" << std::endl;
        std::cout << "✓ AST generated successfully" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}

/*
Example test files you can create:

1. test_simple.cpp:
int x = 5 + 3 * 2;
cout << x;

2. test_control.cpp:
int x = 10;
if (x > 5) {
    cout << "x is greater than 5";
} else {
    cout << "x is not greater than 5";
}

3. test_loop.cpp:
int i = 0;
while (i < 10) {
    cout << i;
    i = i + 1;
}

4. test_complex.cpp:
int a = 5;
int b = 10;
int result = (a + b) * 2 - 3;
cout << "Result: " << result;
if (result > 20) {
    cout << "Result is large";
}
*/
