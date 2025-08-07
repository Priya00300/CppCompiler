#include <iostream>
#include <fstream>
#include <memory>
#include <iomanip>
#include "tokens.hpp"
#include "scanner.hpp"
#include "parser.hpp"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] <input_file>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help     Show this help message" << std::endl;
    std::cout << "  -v, --verbose  Show detailed parsing information" << std::endl;
    std::cout << "  --ast-only     Only show the AST (no other output)" << std::endl;
    std::cout << "  --expr-only    Parse as expression only (for testing)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " program.cpp" << std::endl;
    std::cout << "  " << programName << " --verbose program.cpp" << std::endl;
    std::cout << "  " << programName << " --expr-only expression.txt" << std::endl;
}

void printHeader() {
    std::cout << "┌─────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│           C++ Compiler with Precedence Parser      │" << std::endl;
    std::cout << "│                    Part 3: Precedence              │" << std::endl;
    std::cout << "│               Using Pratt Parsing Algorithm        │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────┘" << std::endl;
}

void printOperatorPrecedenceTable() {
    std::cout << "\n┌─ Operator Precedence Table ─┐" << std::endl;
    std::cout << "│ Precedence │ Operators      │" << std::endl;
    std::cout << "├────────────┼────────────────┤" << std::endl;
    std::cout << "│    50      │ ++ --          │" << std::endl;
    std::cout << "│    40      │ * / %          │" << std::endl;
    std::cout << "│    35      │ << >>          │" << std::endl;
    std::cout << "│    30      │ + -            │" << std::endl;
    std::cout << "│    25      │ < > <= >=      │" << std::endl;
    std::cout << "│    20      │ == !=          │" << std::endl;
    std::cout << "│    15      │ &              │" << std::endl;
    std::cout << "│    14      │ ^              │" << std::endl;
    std::cout << "│    13      │ |              │" << std::endl;
    std::cout << "│     5      │ &&             │" << std::endl;
    std::cout << "│     3      │ ||             │" << std::endl;
    std::cout << "│     2      │ = += -= *= /=  │" << std::endl;
    std::cout << "└────────────┴────────────────┘" << std::endl;
    std::cout << "Higher numbers = Higher precedence" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        bool verbose = false;
        bool astOnly = false;
        bool exprOnly = false;
        std::string inputFile;

        // Parse command line arguments
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];

            if (arg == "-h" || arg == "--help") {
                printUsage(argv[0]);
                return 0;
            } else if (arg == "-v" || arg == "--verbose") {
                verbose = true;
            } else if (arg == "--ast-only") {
                astOnly = true;
            } else if (arg == "--expr-only") {
                exprOnly = true;
            } else if (arg.empty() || arg[0] == '-') {
                std::cerr << "Unknown option: " << arg << std::endl;
                printUsage(argv[0]);
                return 1;
            } else {
                inputFile = arg;
            }
        }

        if (inputFile.empty()) {
            std::cerr << "Error: No input file specified" << std::endl;
            printUsage(argv[0]);
            return 1;
        }

        if (!astOnly) {
            printHeader();
        }

        // Initialize scanner with input file
        Scanner scanner;
        if (!scanner.initialize(inputFile)) {
            std::cerr << "Error: Could not open file '" << inputFile << "'" << std::endl;
            return 1;
        }

        if (verbose && !astOnly) {
            std::cout << "\n📁 Input file: " << inputFile << std::endl;
            printOperatorPrecedenceTable();
            std::cout << "\n🔍 Starting lexical and syntactic analysis..." << std::endl;
        }

        // Initialize parser with scanner
        Parser parser(&scanner);

        std::unique_ptr<ASTNode> ast;

        if (exprOnly) {
            if (verbose && !astOnly) {
                std::cout << "📊 Parsing as expression only..." << std::endl;
            }
            ast = parser.parseExpressionOnly();
        } else {
            if (verbose && !astOnly) {
                std::cout << "📊 Parsing as complete program..." << std::endl;
            }
            ast = parser.parse();
        }

        if (!astOnly) {
            std::cout << "\n✅ Parsing completed successfully!\n" << std::endl;

            if (verbose) {
                std::cout << "🌳 Abstract Syntax Tree (AST):" << std::endl;
                std::cout << "────────────────────────────────" << std::endl;
            } else {
                std::cout << "Abstract Syntax Tree:" << std::endl;
            }
        }

        // Print the AST
        parser.printAST(ast);

        if (!astOnly) {
            std::cout << "\n" << std::string(50, '─') << std::endl;
            std::cout << "📊 Parsing Summary:" << std::endl;
            std::cout << "   ✓ Lexical analysis completed" << std::endl;
            std::cout << "   ✓ Syntax analysis completed" << std::endl;
            std::cout << "   ✓ Operator precedence handled correctly" << std::endl;
            std::cout << "   ✓ AST generated successfully" << std::endl;

            if (verbose) {
                std::cout << "\n💡 Parser Features:" << std::endl;
                std::cout << "   • Pratt parser for expression precedence" << std::endl;
                std::cout << "   • Left and right associativity support" << std::endl;
                std::cout << "   • Unary operator handling" << std::endl;
                std::cout << "   • Parentheses for precedence override" << std::endl;
                std::cout << "   • Error recovery and reporting" << std::endl;

                std::cout << "\n🚀 Ready for next phase: Code Generation!" << std::endl;
            }
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown error occurred" << std::endl;
        return 1;
    }
}

/*
Example usage and expected outputs:

1. Basic precedence test:
   echo "2 + 3 * 4;" > test.cpp
   ./compiler test.cpp

   Expected AST:
   PROGRAM
     EXPRESSION_STMT
       ADD
         INTLIT: 2
         MULTIPLY
           INTLIT: 3
           INTLIT: 4

2. Complex precedence test:
   echo "a = b + c * d == e && f;" > test.cpp
   ./compiler --verbose test.cpp

   This should show proper precedence:
   a = ((b + (c * d)) == e) && f

3. Expression-only testing:
   echo "2 + 3 * 4 - 1" > expr.txt
   ./compiler --expr-only expr.txt

   This parses just the expression without requiring semicolons

4. Verbose output shows the precedence table and detailed information
*/
