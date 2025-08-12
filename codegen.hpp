#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "parser.hpp"
#include "symboltable.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

class CodeGenerator {
private:
    std::ostream* output;           // Output stream (file or cout)
    bool ownsStream;                // Whether we own the output stream
    int nextRegister;               // Next available register number
    std::vector<bool> usedRegisters; // Track which registers are in use
    int labelCounter;               // For generating unique labels

    // Symbol table for variable management
    SymbolTable symbolTable;
    int stackOffset;               // Current stack offset

    // Register management
    static const int MAX_REGISTERS = 8;  // Using r8-r15 for temporaries
    static const std::string registers[];

    // Private helper methods
    int allocateRegister();
    void freeRegister(int reg);
    void freeAllRegisters();
    bool isValidRegister(int reg);
    std::string getRegisterName(int reg);

    // Variable management methods
    void addVariable(const std::string& name);
    int getVariableOffset(const std::string& name);
    void loadVariable(int reg, const std::string& name);
    void storeVariable(const std::string& name, int reg);

    // Code generation helpers
    void generateBinaryOp(ASTNodeType op, int leftReg, int rightReg);
    void generateUnaryOp(ASTNodeType op, int reg);

public:
    // Constructors and destructor
    CodeGenerator(std::ostream* out);
    CodeGenerator(const std::string& filename);
    ~CodeGenerator();

    // Main code generation entry point
    void generateCode(const std::unique_ptr<ASTNode>& ast);

    // Generate code for different AST node types
    int generateExpression(const std::unique_ptr<ASTNode>& node);
    void generateStatement(const std::unique_ptr<ASTNode>& node);
    void generateProgram(const std::unique_ptr<ASTNode>& node);

    // Assembly output helpers
    void emit(const std::string& instruction);
    void emitComment(const std::string& comment);
    void emitLabel(const std::string& label);
    std::string generateLabel(const std::string& prefix = "L");

    // Code generation structure
    void generatePreamble();
    void generatePostamble(int exitCode = 0);

    // Load immediate values
    void loadImmediate(int reg, int value);
    void loadImmediate(int reg, float value);

    // Error handling
    void error(const std::string& message);

    // Symbol table access
    SymbolTable& getSymbolTable() { return symbolTable; }
};

#endif // CODEGEN_HPP
