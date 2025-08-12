#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <string>
#include <unordered_map>

// Symbol types
enum class SymbolType {
    INTEGER,
    FLOAT,
    CHAR,
    BOOLEAN,
    VOID
};

// Symbol structure
struct Symbol {
    std::string name;
    SymbolType type;
    int offset;        // Stack offset from base pointer
    bool initialized;  // Whether the variable has been initialized
    int scope;        // Scope level (for nested scopes)

    Symbol() : type(SymbolType::INTEGER), offset(0), initialized(false), scope(0) {}

    Symbol(const std::string& n, SymbolType t, int off, int sc = 0)
        : name(n), type(t), offset(off), initialized(false), scope(sc) {}
};

// Symbol table class
class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> symbols;
    int currentOffset;  // Current stack offset
    int currentScope;   // Current scope level

public:
    SymbolTable() : currentOffset(-8), currentScope(0) {}

    // Add a new symbol
    bool addSymbol(const std::string& name, SymbolType type);

    // Find a symbol
    Symbol* findSymbol(const std::string& name);

    // Check if symbol exists
    bool exists(const std::string& name) const;

    // Mark symbol as initialized
    void markInitialized(const std::string& name);

    // Get all symbols (for debugging)
    const std::unordered_map<std::string, Symbol>& getAllSymbols() const;

    // Clear symbol table
    void clear();

    // Scope management
    void enterScope();
    void exitScope();

    // Get current offset
    int getCurrentOffset() const { return currentOffset; }
};

#endif // SYMBOLTABLE_HPP
