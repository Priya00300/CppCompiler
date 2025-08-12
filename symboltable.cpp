#include "symboltable.hpp"
#include <iostream>

bool SymbolTable::addSymbol(const std::string& name, SymbolType type) {
    if (exists(name)) {
        return false; // Symbol already exists
    }

    symbols.emplace(name, Symbol(name, type, currentOffset, currentScope));
    currentOffset -= 8; // Each variable takes 8 bytes on stack
    return true;
}

Symbol* SymbolTable::findSymbol(const std::string& name) {
    auto it = symbols.find(name);
    return (it != symbols.end()) ? &it->second : nullptr;
}

bool SymbolTable::exists(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

void SymbolTable::markInitialized(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        it->second.initialized = true;
    }
}

const std::unordered_map<std::string, Symbol>& SymbolTable::getAllSymbols() const {
    return symbols;
}

void SymbolTable::clear() {
    symbols.clear();
    currentOffset = -8;
    currentScope = 0;
}

void SymbolTable::enterScope() {
    currentScope++;
}

void SymbolTable::exitScope() {
    // Remove all symbols from current scope
    auto it = symbols.begin();
    while (it != symbols.end()) {
        if (it->second.scope >= currentScope) {
            it = symbols.erase(it);
        } else {
            ++it;
        }
    }

    if (currentScope > 0) {
        currentScope--;
    }
}
