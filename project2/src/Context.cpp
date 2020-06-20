#include "Context.h"

Context::~Context() {
  for (auto t : types) {
    delete t;
  }
}

const BasicType *Context::TYPE_INT = new BasicType("int");
const BasicType *Context::TYPE_DOUBLE = new BasicType("double");
const BasicType *Context::TYPE_BOOL = new BasicType("bool");
const BasicType *Context::TYPE_VOID = new BasicType("void");

Context* Context::getDefaultContext() {
  Context *c = new Context();
  c->types.push_back(TYPE_INT);
  c->types.push_back(TYPE_DOUBLE);
  c->types.push_back(TYPE_BOOL);
  c->types.push_back(TYPE_VOID);
  return c;
}

bool Context::addSymbol(const std::string& id, const BasicType *type) {
  if (findSymbolInThis(id) != nullptr) return false;

  basicSymbols.emplace(id, type);
  return true;
}

const BasicType* Context::findSymbol(const std::string& id) {
  auto res = basicSymbols.find(id);

  if (res == basicSymbols.end()) {
    if (parent == nullptr) return nullptr;
    return parent->findSymbol(id);
  }

  return (*res).second;
}

const BasicType *Context::findSymbolInThis(const std::string& id) {
  auto res = basicSymbols.find(id);
  if (res == basicSymbols.end()) {
    return nullptr;
  }

  return (*res).second;
}

bool Context::addTypeDeclaration(const std::string& id) {
  if (findBasicType(id) != nullptr) return false;

  types.push_back(new BasicType(id));
  return true;
}

bool Context::addStructDeclaration(const std::string& id, const std::map<const std::string, const BasicType *> members) {
  if (findBasicType(id) != nullptr) return false;

  types.push_back(new StructType(id, members));
  return true;
}

const BasicType* Context::findBasicType(const std::string& id) {
  for (auto type : types) {
    if (type->id == id) {
      return type;
    }
  }

  if (parent == nullptr) return nullptr;
  return parent->findBasicType(id);
}


bool Context::addFunction(const std::string& id, const FunctionType *type) {
  if (findFunctionInThis(id) != nullptr) return false;

  functionSymbols.emplace(id, type);
  return true;
}

const FunctionType* Context::findFunction(const std::string& id) {
  auto res = functionSymbols.find(id);
  if (res == functionSymbols.end()) {
    if (parent == nullptr) return nullptr;
    return parent->findFunction(id);
  }

  return (*res).second;
}

const FunctionType *Context::findFunctionInThis(const std::string& id) {
  auto res = functionSymbols.find(id);
  if (res == functionSymbols.end()) return nullptr;
  return (*res).second;
}

const std::vector<const BasicType *> Context::getKnownTypes() const {
  return types;
}

Context* Context::createChildContext() {
  Context *child = new Context();
  child->parent = this;
  return child;
}