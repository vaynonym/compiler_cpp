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

void Context::addSymbol(const std::string& id, const BasicType *type) {
  if (basicSymbols.find(id) != basicSymbols.end()) {
    // TODO: Error reporting
    return;
  }

  basicSymbols.emplace(id, type);
}

const BasicType* Context::findSymbol(const std::string& id) {
  auto res = basicSymbols.find(id);

  if (res == basicSymbols.end()) {
    if (parent == nullptr) return nullptr;
    return parent->findSymbol(id);
  }

  return (*res).second;
}

void Context::addTypeDeclaration(const std::string& id) {
  // TODO: check if exists
  types.push_back(new BasicType(id));
}

const BasicType* Context::findBasicType(const std::string& id) {
  for (auto type : types) {
    if (type->Id == id) {
      return type;
    }
  }

  if (parent == nullptr) return nullptr;
  return parent->findBasicType(id);
}

Context* Context::createChildContext() {
  Context *child = new Context();
  child->parent = this;
  return child;
}