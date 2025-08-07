#include <iostream>
#include <fstream>
#include <memory>
#include <iomanip>
#include "tokens.hpp"
#include "scanner.hpp"
#include "parser.hpp"
#include "codegen.hpp"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] <input_file> [output_file]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help        Show this help message" << std::endl;
    std::cout << "  -v, --verbose     Show detailed compilation information" << std::endl;
    std::cout << "  --ast-only        Only show the AST (no code generation)" << std::endl;
    std::cout << "  --parse-only      Only parse (no code generation)" << std::endl;
    std::cout << "  --expr-only       Parse as expression only (for testing)" << std::endl;
    std::cout << "  -o <file>         Specify output assembly file" << std::endl;
    std::cout << "  --to-stdout       Output assembly to stdout" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " program.cpp                    # Output to program.s" << std::endl;
    std::cout << "  " << programName << " program.cpp -o output.s        # Output to output.s" << std::endl;
    std::cout << "  " << programName << " --to-stdout program.cpp        # Output to stdout" << std::endl;
    std::cout << "  " << programName << " --verbose program.cpp          # Show detailed info" << std::endl;
    std::cout << "  " << programName << " --ast-only program.cpp         # Show AST only" << std::endl;
    std::cout << std::endl;
    std::cout << "Assembly and Execution:" << std::endl;
    std::cout << "  as -64 output.s -o output.o                        # Assemble" << std::endl;
    std::cout << "  ld output.o -o output                              # Link" << std::endl;
    std::cout << "  ./output; echo $?                                  # Run and show exit code" << std::endl;
}

void printHeader() {
    std::cout << "┌─────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│           C++ Compiler with Code Generation        │" << std::endl;
    std::cout << "│                 Part 4: Assembly                   │" << std::endl;
    std::cout << "│              AST → x86-64 Assembly                 │" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────┘" << std::endl;
}

void printCompilationSteps() {
    std::cout << "\n┌─ Compilation Pipeline ─┐" << std::endl;
    std::cout << "│ 1. Lexical Analysis    │ ← Tokenizing" << std::endl;
    std::cout << "│ 2. Syntax Analysis     │ ← Parsing" << std::endl;
    std::cout << "│ 3. AST Generation      │ ← Abstract Syntax Tree" << std::endl;
    std::cout << "│ 4. Code Generation     │ ← x86-64 Assembly" << std::endl;
    std::cout << "│ 5. Assembly            │ ← as -64 file.s -o file.o" << std::endl;
    std::cout << "│ 6. Linking             │ ← ld file.o -o executable" << std::endl;
    std::cout << "└────────────────────────┘" << std::endl;
}

std::string getOutputFilename(const std::string& inputFile, const std::string& outputFile) {
    if (!outputFile.empty()) {
        return outputFile;
    }

    // Generate output filename from input filename
    size_t lastDot = inputFile.find_last_of('.');
    if (lastDot != std::string::npos) {
        return inputFile.substr(0, lastDot) + ".s";
    }
    return inputFile + ".s";
}

int main(int argc, char* argv[]) {
    try {
        bool verbose = false;
        bool astOnly = false;
        bool parseOnly = false;
        bool exprOnly = false;
        bool toStdout = false;
        std::string inputFile;
        std::string outputFile;

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
            } else if (arg == "--parse-only") {
                parseOnly = true;
            } else if (arg == "--expr-only") {
                exprOnly = true;
            } else if (arg == "--to-stdout") {
                toStdout = true;
            } else if (arg == "-o" && i + 1 < argc) {
                outputFile = argv[++i];
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

        if (!astOnly && !parseOnly) {
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
            printCompilationSteps();
            std::cout << "\n🔍 Phase 1: Lexical Analysis..." << std::endl;
        }

        // Initialize parser with scanner
        Parser parser(&scanner);

        if (verbose && !astOnly) {
            std::cout << "✓ Lexical analysis completed" << std::endl;
            std::cout << "\n📊 Phase 2: Syntax Analysis..." << std::endl;
        }

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

        if (verbose && !astOnly) {
            std::cout << "✓ Syntax analysis completed" << std::endl;
            std::cout << "\n🌳 Phase 3: AST Generation completed" << std::endl;
        }

        if (!astOnly && !parseOnly) {
            if (verbose) {
                std::cout << "\n🔧 Phase 4: Code Generation..." << std::endl;
            }

            // Determine output location
            if (toStdout) {
                if (verbose) {
                    std::cout << "📤 Generating assembly code to stdout" << std::endl;
                    std::cout << "\n" << std::string(50, '=') << std::endl;
                    std::cout << "GENERATED ASSEMBLY CODE:" << std::endl;
                    std::cout << std::string(50, '=') << std::endl;
                }

                CodeGenerator codegen(&std::cout);
                codegen.generateCode(ast);

            } else {
                std::string finalOutputFile = getOutputFilename(inputFile, outputFile);

                if (verbose) {
                    std::cout << "📤 Output file: " << finalOutputFile << std::endl;
                    std::cout << "🔧 Generating x86-64 assembly code..." << std::endl;
                }

                CodeGenerator codegen(finalOutputFile);
                codegen.generateCode(ast);

                if (verbose) {
                    std::cout << "✓ Code generation completed" << std::endl;

                    std::cout << "\n🚀 Next Steps:" << std::endl;
                    std::cout << "   1. Assemble:  as -64 " << finalOutputFile << " -o " <<
                                 finalOutputFile.substr(0, finalOutputFile.find_last_of('.')) << ".o" << std::endl;
                    std::cout << "   2. Link:      ld " <<
                                 finalOutputFile.substr(0, finalOutputFile.find_last_of('.')) << ".o -o " <<
                                 finalOutputFile.substr(0, finalOutputFile.find_last_of('.')) << std::endl;
                    std::cout << "   3. Run:       ./" <<
                                 finalOutputFile.substr(0, finalOutputFile.find_last_of('.')) << std::endl;
                    std::cout << "   4. Check:     echo $?  # Shows exit code (result)" << std::endl;
                } else {
                    std::cout << "✅ Assembly generated: " << finalOutputFile << std::endl;
                }
            }
        }

        // Always show AST if requested or if verbose
        if (astOnly || verbose || parseOnly) {
            if (!astOnly && !parseOnly) {
                std::cout << "\n🌳 Abstract Syntax Tree:" << std::endl;
                std::cout << "────────────────────────────────" << std::endl;
            }
            parser.printAST(ast);
        }

        if (!astOnly && !parseOnly && !verbose) {
            std::cout << "\n✅ Compilation completed successfully!" << std::endl;

            if (!toStdout) {
                std::string finalOutputFile = getOutputFilename(inputFile, outputFile);
                std::cout << "📁 Assembly file generated: " << finalOutputFile << std::endl;

                std::cout << "\n📋 To assemble and run:" << std::endl;
                std::cout << "   as -64 " << finalOutputFile << " -o output.o && " <<
                             "ld output.o -o output && ./output; echo \"Exit code: $?\"" << std::endl;
            }
        }

        if (verbose && !astOnly && !parseOnly) {
            std::cout << "\n" << std::string(50, '─') << std::endl;
            std::cout << "📊 Compilation Summary:" << std::endl;
            std::cout << "   ✓ Lexical analysis (tokenizing)" << std::endl;
            std::cout << "   ✓ Syntax analysis (parsing)" << std::endl;
            std::cout << "   ✓ AST generation" << std::endl;
            std::cout << "   ✓ Code generation (x86-64 assembly)" << std::endl;

            std::cout << "\n💡 Compiler Features:" << std::endl;
            std::cout << "   • Pratt parser for operator precedence" << std::endl;
            std::cout << "   • x86-64 assembly code generation" << std::endl;
            std::cout << "   • Register allocation and management" << std::endl;
            std::cout << "   • Binary and unary operator support" << std::endl;
            std::cout << "   • Integer and float literal support" << std::endl;
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

1. Basic expression compilation:
   echo "2 + 3 * 4;" > test.cpp
   ./compiler test.cpp
   as -64 test.s -o test.o
   ld test.o -o test
   ./test; echo $?
   # Should output: 14 (2 + 12)

2. Complex expression:
   echo "10 - 2 * 3 + 1;" > complex.cpp
   ./compiler --verbose complex.cpp
   # Shows full compilation pipeline

3. Output to stdout:
   ./compiler --to-stdout test.cpp
   # Generates assembly directly to terminal

4. Custom output file:
   ./compiler test.cpp -o mycode.s
   # Generates assembly to mycode.s

5. AST only (for debugging):
   ./compiler --ast-only test.cpp
   # Shows only the Abstract Syntax Tree

Usage patterns:
- ./compiler file.cpp              → file.s
- ./compiler file.cpp -o out.s     → out.s
- ./compiler --to-stdout file.cpp  → stdout
- ./compiler --verbose file.cpp    → detailed output + file.s
- ./compiler --ast-only file.cpp   → AST only, no assembly
*/
