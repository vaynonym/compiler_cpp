#include "CodeGenContext.h"

CodeGenContext::~CodeGenContext() { }

bool CodeGenContext::addSymbol(const std::string& id, llvm::Value *value) {
  if (findSymbolInThis(id) != nullptr) return false;

  knownSymbols.emplace(id, value);
  return true;
}

llvm::Value *CodeGenContext::findSymbol(const std::string& id) {
  auto res = knownSymbols.find(id);

  if (res == knownSymbols.end()) {
    if (parent == nullptr) return nullptr;
    return parent->findSymbol(id);
  }

  return (*res).second;
}

llvm::Value *CodeGenContext::findSymbolInThis(const std::string& id) {
  auto res = knownSymbols.find(id);
  if (res == knownSymbols.end()) {
    return nullptr;
  }

  return (*res).second;
}

CodeGenContext *CodeGenContext::createChildContext() {
  CodeGenContext *child = new CodeGenContext();
  child->parent = this;
  return child;
}