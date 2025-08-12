#include "codegen.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unordered_map>

// x86-64 registers we'll use for temporaries (r8-r15)
const std::string CodeGenerator::registers[] = {
    "%r8",  "%r9",  "%r10", "%r11",
    "%r12", "%r13", "%r14", "%r15"
};

CodeGenerator::CodeGenerator(std::ostream* out)
    : output(out), ownsStream(false), nextRegister(0), labelCounter(0), stackOffset(0) {
    usedRegisters.resize(MAX_REGISTERS, false);
}

CodeGenerator::CodeGenerator(const std::string& filename)
    : ownsStream(true), nextRegister(0), labelCounter(0), stackOffset(0) {
    output = new std::ofstream(filename);
    if (!static_cast<std::ofstream*>(output)->is_open()) {
        delete output;
        throw std::runtime_error("Cannot open output file: " + filename);
    }
    usedRegisters.resize(MAX_REGISTERS, false);
}

CodeGenerator::~CodeGenerator() {
    if (ownsStream && output) {
        delete output;
    }
}

void CodeGenerator::error(const std::string& message) {
    throw std::runtime_error("Code generation error: " + message);
}

int CodeGenerator::allocateRegister() {
    for (int i = 0; i < MAX_REGISTERS; i++) {
        if (!usedRegisters[i]) {
            usedRegisters[i] = true;
            return i;
        }
    }
    error("No registers available");
    return -1;
}

void CodeGenerator::freeRegister(int reg) {
    if (isValidRegister(reg)) {
        usedRegisters[reg] = false;
    }
}

void CodeGenerator::freeAllRegisters() {
    std::fill(usedRegisters.begin(), usedRegisters.end(), false);
}

bool CodeGenerator::isValidRegister(int reg) {
    return reg >= 0 && reg < MAX_REGISTERS;
}

std::string CodeGenerator::getRegisterName(int reg) {
    if (isValidRegister(reg)) {
        return registers[reg];
    }
    error("Invalid register number: " + std::to_string(reg));
    return "";
}

void CodeGenerator::emit(const std::string& instruction) {
    *output << "    " << instruction << std::endl;
}

void CodeGenerator::emitComment(const std::string& comment) {
    *output << "    # " << comment << std::endl;
}

void CodeGenerator::emitLabel(const std::string& label) {
    *output << label << ":" << std::endl;
}

std::string CodeGenerator::generateLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter++);
}

void CodeGenerator::loadImmediate(int reg, int value) {
    emit("movq $" + std::to_string(value) + ", " + getRegisterName(reg));
}

void CodeGenerator::loadImmediate(int reg, float value) {
    // For simplicity, we'll convert float to int for now
    // In a real compiler, you'd handle floating point properly
    emit("movq $" + std::to_string(static_cast<int>(value)) + ", " + getRegisterName(reg));
}

// Symbol table management - UPDATED for SymbolTable class
void CodeGenerator::addVariable(const std::string& name) {
    if (!symbolTable.addSymbol(name, SymbolType::INTEGER)) {
        error("Variable '" + name + "' already declared");
    }
    emitComment("Variable '" + name + "' declared");
}

int CodeGenerator::getVariableOffset(const std::string& name) {
    Symbol* sym = symbolTable.findSymbol(name);
    if (!sym) {
        error("Variable '" + name + "' not declared");
    }
    return sym->offset;
}

void CodeGenerator::loadVariable(int reg, const std::string& name) {
    Symbol* sym = symbolTable.findSymbol(name);
    if (!sym) {
        error("Variable '" + name + "' not declared");
    }
    if (!sym->initialized) {
        error("Variable '" + name + "' used before initialization");
    }

    emit("movq " + std::to_string(sym->offset) + "(%rbp), " + getRegisterName(reg));
    emitComment("Load variable '" + name + "'");
}

void CodeGenerator::storeVariable(const std::string& name, int reg) {
    Symbol* sym = symbolTable.findSymbol(name);
    if (!sym) {
        error("Variable '" + name + "' not declared");
    }

    emit("movq " + getRegisterName(reg) + ", " + std::to_string(sym->offset) + "(%rbp)");
    symbolTable.markInitialized(name);
    emitComment("Store to variable '" + name + "'");
}

void CodeGenerator::generateBinaryOp(ASTNodeType op, int leftReg, int rightReg) {
    std::string leftRegName = getRegisterName(leftReg);
    std::string rightRegName = getRegisterName(rightReg);

    switch (op) {
        case ASTNodeType::ADD:
            emit("addq " + rightRegName + ", " + leftRegName);
            break;

        case ASTNodeType::SUBTRACT:
            emit("subq " + rightRegName + ", " + leftRegName);
            break;

        case ASTNodeType::MULTIPLY:
            emit("imulq " + rightRegName + ", " + leftRegName);
            break;

        case ASTNodeType::DIVIDE:
            // x86-64 division - save registers that might be used
            emit("pushq %rax");           // Save rax
            emit("pushq %rdx");           // Save rdx
            emit("movq " + leftRegName + ", %rax");
            emit("cqto");                 // Sign extend rax to rdx:rax
            emit("idivq " + rightRegName);
            emit("movq %rax, " + leftRegName);  // Move result back
            emit("popq %rdx");            // Restore rdx
            emit("popq %rax");            // Restore rax
            break;

        case ASTNodeType::MODULO:
            // Similar to division, but result is in rdx
            emit("pushq %rax");           // Save rax
            emit("pushq %rdx");           // Save rdx
            emit("movq " + leftRegName + ", %rax");
            emit("cqto");
            emit("idivq " + rightRegName);
            emit("movq %rdx, " + leftRegName);  // Move remainder back
            emit("popq %rdx");            // Restore rdx
            emit("popq %rax");            // Restore rax
            break;

        case ASTNodeType::EQ:
            emit("cmpq " + rightRegName + ", " + leftRegName);
            emit("sete %al");
            emit("movzbq %al, " + leftRegName);
            break;

        case ASTNodeType::NE:
            emit("cmpq " + rightRegName + ", " + leftRegName);
            emit("setne %al");
            emit("movzbq %al, " + leftRegName);
            break;

        case ASTNodeType::LT:
            emit("cmpq " + rightRegName + ", " + leftRegName);
            emit("setl %al");
            emit("movzbq %al, " + leftRegName);
            break;

        case ASTNodeType::GT:
            emit("cmpq " + rightRegName + ", " + leftRegName);
            emit("setg %al");
            emit("movzbq %al, " + leftRegName);
            break;

        case ASTNodeType::LE:
            emit("cmpq " + rightRegName + ", " + leftRegName);
            emit("setle %al");
            emit("movzbq %al, " + leftRegName);
            break;

        case ASTNodeType::GE:
            emit("cmpq " + rightRegName + ", " + leftRegName);
            emit("setge %al");
            emit("movzbq %al, " + leftRegName);
            break;

        case ASTNodeType::AND: {
            // Logical AND - if left is 0, result is 0; otherwise result is right != 0
            emit("testq " + leftRegName + ", " + leftRegName);
            std::string andZeroLabel = generateLabel("and_zero_");
            std::string endLabel = generateLabel("end_and_");
            emit("jz " + andZeroLabel);  // Jump if left is 0

            // Left is non-zero, so result depends on right
            emit("testq " + rightRegName + ", " + rightRegName);
            emit("setnz %al");
            emit("movzbq %al, " + leftRegName);
            emit("jmp " + endLabel);

            emitLabel(andZeroLabel);
            emit("movq $0, " + leftRegName);
            emitLabel(endLabel);
            break;
        }

        case ASTNodeType::OR: {
            // Logical OR - if left is non-zero, result is 1; otherwise result is right != 0
            emit("testq " + leftRegName + ", " + leftRegName);
            std::string orOneLabel = generateLabel("or_one_");
            std::string endOrLabel = generateLabel("end_or_");
            emit("jnz " + orOneLabel);  // Jump if left is non-zero

            // Left is zero, so result depends on right
            emit("testq " + rightRegName + ", " + rightRegName);
            emit("setnz %al");
            emit("movzbq %al, " + leftRegName);
            emit("jmp " + endOrLabel);

            emitLabel(orOneLabel);
            emit("movq $1, " + leftRegName);
            emitLabel(endOrLabel);
            break;
        }

        default:
            error("Unsupported binary operation");
    }
}

void CodeGenerator::generateUnaryOp(ASTNodeType op, int reg) {
    std::string regName = getRegisterName(reg);

    switch (op) {
        case ASTNodeType::NEGATE:
            emit("negq " + regName);
            break;

        case ASTNodeType::POSITIVE:
            // No operation needed for unary plus
            break;

        case ASTNodeType::NOT:
            // Logical NOT - if reg is 0, result is 1; otherwise result is 0
            emit("testq " + regName + ", " + regName);
            emit("setz %al");
            emit("movzbq %al, " + regName);
            break;

        default:
            error("Unsupported unary operation");
    }
}

int CodeGenerator::generateExpression(const std::unique_ptr<ASTNode>& node) {
    if (!node) {
        error("Null AST node");
        return -1;
    }

    switch (node->type) {
        case ASTNodeType::INTLIT: {
            int reg = allocateRegister();
            loadImmediate(reg, node->intValue);
            emitComment("Load integer literal: " + std::to_string(node->intValue));
            return reg;
        }

        case ASTNodeType::FLOATLIT: {
            int reg = allocateRegister();
            loadImmediate(reg, node->floatValue);
            emitComment("Load float literal: " + std::to_string(node->floatValue));
            return reg;
        }

        case ASTNodeType::IDENTIFIER: {
            int reg = allocateRegister();
            loadVariable(reg, node->value);
            return reg;
        }

        case ASTNodeType::ASSIGN: {
            if (!node->left || !node->right) {
                error("Assignment missing operands");
                return -1;
            }

            // The left side should be an identifier
            if (node->left->type != ASTNodeType::IDENTIFIER) {
                error("Left side of assignment must be a variable");
                return -1;
            }

            // Generate code for the right side (the value to assign)
            int valueReg = generateExpression(node->right);

            // Store the value to the variable
            storeVariable(node->left->value, valueReg);

            // Assignment expression returns the assigned value
            return valueReg;
        }

        // Binary operations
        case ASTNodeType::ADD:
        case ASTNodeType::SUBTRACT:
        case ASTNodeType::MULTIPLY:
        case ASTNodeType::DIVIDE:
        case ASTNodeType::MODULO:
        case ASTNodeType::EQ:
        case ASTNodeType::NE:
        case ASTNodeType::LT:
        case ASTNodeType::GT:
        case ASTNodeType::LE:
        case ASTNodeType::GE:
        case ASTNodeType::AND:
        case ASTNodeType::OR: {
            if (!node->left || !node->right) {
                error("Binary operation missing operands");
                return -1;
            }

            int leftReg = generateExpression(node->left);
            int rightReg = generateExpression(node->right);

            emitComment("Binary operation: " +
                       getRegisterName(leftReg) + " " +
                       (node->type == ASTNodeType::ADD ? "+" :
                        node->type == ASTNodeType::SUBTRACT ? "-" :
                        node->type == ASTNodeType::MULTIPLY ? "*" :
                        node->type == ASTNodeType::DIVIDE ? "/" :
                        node->type == ASTNodeType::MODULO ? "%" : "op") + " " +
                       getRegisterName(rightReg));

            generateBinaryOp(node->type, leftReg, rightReg);

            freeRegister(rightReg);
            return leftReg;
        }

        // Unary operations
        case ASTNodeType::NEGATE:
        case ASTNodeType::POSITIVE:
        case ASTNodeType::NOT: {
            if (!node->left) {
                error("Unary operation missing operand");
                return -1;
            }

            int reg = generateExpression(node->left);

            emitComment(std::string("Unary operation: ") +
                       (node->type == ASTNodeType::NEGATE ? "-" :
                        node->type == ASTNodeType::POSITIVE ? "+" : "!") + " " +
                       getRegisterName(reg));

            generateUnaryOp(node->type, reg);
            return reg;
        }

        default:
            error("Unsupported expression type");
            return -1;
    }
}

void CodeGenerator::generateStatement(const std::unique_ptr<ASTNode>& node) {
    if (!node) return;

    switch (node->type) {
        case ASTNodeType::VAR_DECL: {
            // Variable declaration - just add to symbol table
            if (!node->value.empty()) {
                addVariable(node->value);
            }
            break;
        }

        case ASTNodeType::EXPRESSION_STMT: {
            if (node->left) {
                int reg = generateExpression(node->left);
                freeRegister(reg);
            }
            break;
        }

        case ASTNodeType::COMPOUND_STMT: {
            for (const auto& child : node->children) {
                generateStatement(child);
            }
            break;
        }

        // For now, we'll ignore other statement types
        case ASTNodeType::COUT_STMT:
        case ASTNodeType::CIN_STMT:
        case ASTNodeType::IF_STMT:
        case ASTNodeType::WHILE_STMT:
        case ASTNodeType::FOR_STMT:
        case ASTNodeType::RETURN_STMT:
            emitComment("Statement type not yet implemented: " + std::to_string(static_cast<int>(node->type)));
            break;

        default:
            emitComment("Unknown statement type: " + std::to_string(static_cast<int>(node->type)));
            break;
    }
}

void CodeGenerator::generateProgram(const std::unique_ptr<ASTNode>& node) {
    if (!node || node->type != ASTNodeType::PROGRAM) {
        error("Expected program node");
        return;
    }

    generatePreamble();

    // If the program has children (statements), generate them
    if (!node->children.empty()) {
        // For a program with statements, evaluate the last expression statement
        // and use its result as the exit code
        int lastExpressionReg = -1;

        for (const auto& child : node->children) {
            if (child->type == ASTNodeType::EXPRESSION_STMT && child->left) {
                // Free the previous expression result if any
                if (lastExpressionReg != -1) {
                    freeRegister(lastExpressionReg);
                }
                // Generate the expression and keep its result
                lastExpressionReg = generateExpression(child->left);
            } else {
                generateStatement(child);
            }
        }

        // Use the last expression's result as exit code, or 0 if none
        if (lastExpressionReg != -1) {
            emit("movq " + getRegisterName(lastExpressionReg) + ", %rax");
            freeRegister(lastExpressionReg);
            generatePostamble(-1);  // -1 means exit code is already in %rax
        } else {
            generatePostamble(0);
        }
    } else {
        // If it's just an expression, evaluate it and exit with its value
        emitComment("Single expression program");
        if (node->left) {
            int reg = generateExpression(node->left);
            emit("movq " + getRegisterName(reg) + ", %rax");  // Move result to return value
            freeRegister(reg);
            generatePostamble(-1);  // -1 means exit code is already in %rax
        } else {
            generatePostamble(0);
        }
    }
}

void CodeGenerator::generatePreamble() {
    *output << "# Generated by C++ Compiler - Code Generation Phase" << std::endl;
    *output << "# x86-64 Assembly Output for Windows (MinGW64)" << std::endl;
    *output << std::endl;

    // Windows-compatible assembly using main instead of _start
    *output << ".text" << std::endl;
    *output << ".globl main" << std::endl;
    *output << std::endl;

    emitLabel("main");
    emitComment("Program start");

    // Set up stack frame for Windows calling convention
    emit("pushq %rbp");
    emit("movq %rsp, %rbp");
    emit("subq $32, %rsp");  // Shadow space for Windows x64 calling convention
}

void CodeGenerator::generatePostamble(int exitCode) {
    emitComment("Program exit");

    if (exitCode != -1) {
        emit("movq $" + std::to_string(exitCode) + ", %rax");
    }
    // If exitCode is -1, assume the exit code is already in %rax

    // Clean up stack frame
    emit("addq $32, %rsp");
    emit("movq %rbp, %rsp");
    emit("popq %rbp");
    emit("ret");  // Return instead of syscall
}

void CodeGenerator::generateCode(const std::unique_ptr<ASTNode>& ast) {
    if (!ast) {
        error("Cannot generate code for null AST");
        return;
    }

    // Reset state
    freeAllRegisters();
    labelCounter = 0;
    stackOffset = 0;
    symbolTable.clear();  // Clear the symbol table (assuming it has a clear method)

    if (ast->type == ASTNodeType::PROGRAM) {
        generateProgram(ast);
    } else {
        // Assume it's a single expression
        generatePreamble();
        emitComment("Single expression evaluation");
        int reg = generateExpression(ast);
        emit("movq " + getRegisterName(reg) + ", %rax");  // Move result to return value
        generatePostamble(-1);  // -1 means exit code is already in %rax
        freeRegister(reg);
    }
}
