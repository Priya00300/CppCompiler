# C++ Compiler

A custom C++ compiler implementation written in C++, supporting a subset of the C++ language features. This project demonstrates compiler construction principles including lexical analysis, parsing, semantic analysis, and code generation.
 
Features

Supported Language Features
- **Data Types**: `int`, `float`, `double`, `char`, `bool`
- **Variables**: Declaration, initialization, and assignment
- **Operators**: Arithmetic (`+`, `-`, `*`, `/`, `%`), comparison (`==`, `!=`, `<`, `>`, `<=`, `>=`), logical (`&&`, `||`, `!`)
- **Control Flow**: `if`/`else` statements, `while` and `for` loops
- **Functions**: Function declaration, definition, and calls
- **Arrays**: One-dimensional array support
- **Input/Output**: Basic `cout` and `cin` operations
- **Comments**: Single-line (`//`) and multi-line (`/* */`) comments

Compiler Pipeline
1. **Lexical Analysis**: Tokenization of source code
2. **Syntax Analysis**: Recursive descent parser
3. **Semantic Analysis**: Type checking and symbol table management
4. **Code Generation**: Assembly code output (x86-64)
5. **Error Reporting**: Comprehensive error messages with line numbers

Installation

### Prerequisites
- GCC or Clang compiler (C++17 or later)
- Make (optional, for build automation)
- Linux/macOS/Windows with WSL

Build Instructions


Clone the repository
git clone https://github.com/yourusername/cpp-compiler.git
cd cpp-compiler

Build the compiler
make
OR compile manually
g++ -std=c++17 -O2 src/*.cpp -o cppcompiler

 Make executable globally accessible (optional)
sudo cp cppcompiler /usr/local/bin/


 

Basic Compilation


 Compile a C++ source file
./cppcompiler input.cpp -o output.asm

With debugging information
./cppcompiler input.cpp -o output.asm -debug

Verbose output
./cppcompiler input.cpp -o output.asm -verbose

 Example Program

Create a file `example.cpp`:


#include <iostream>

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    int num = 5;
    int result = factorial(num);
    cout << "Factorial of " << num << " is " << result << endl;
    return 0;
}


Compile it:


./cppcompiler example.cpp -o example.asm



Key Components

- Lexer: Converts source code into tokens using finite state automaton
- Parser: Builds Abstract Syntax Tree using recursive descent parsing
- Semantic Analyzer: Performs type checking and scope resolution
- Code Generator: Translates AST to target assembly code
- Symbol Table: Manages variable and function declarations
- Error Handler: Provides meaningful error messages and recovery

Testing

Run Test Suite


# Run all tests
make test

# Run specific test category
make test-basic
make test-advanced
make test-errors

# Run individual test
./cppcompiler tests/basic/variables.cpp -o temp.asm






