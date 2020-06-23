#pragma once

#include <string>
#include <map>
#include <vector>

#include "llvm/IR/Value.h"

class CodeGenContext {
  private:
    std::map<const std::string, llvm::Value *> knownSymbols;

    llvm::Value *findSymbolInThis(const std::string& id);

  public:
    CodeGenContext* parent;

    ~CodeGenContext();

    bool addSymbol(const std::string& id, llvm::Value *value);
    void setSymbol(const std::string& id, llvm::Value *value);
    llvm::Value* findSymbol(const std::string& id);

    CodeGenContext* createChildContext();
};