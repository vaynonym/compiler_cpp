#pragma once

#include <string>
#include <map>
#include <vector>
#include "Types.h"

class Context {
  std::vector<const BasicType *> types;
  std::map<std::string, const BasicType *> basicSymbols;
  std::map<std::string, FunctionType *> functionSymbols;

  Context* parent;

  private:
    Context() {}

  public:
    static const BasicType *TYPE_INT;
    static const BasicType *TYPE_DOUBLE;
    static const BasicType *TYPE_BOOL;
    static const BasicType *TYPE_VOID;

    // Must only ever be called once during the program's lifetime, because deleting the default
    // context also deletes the basic primitive types.
    static Context* getDefaultContext();

    ~Context();

    void addSymbol(const std::string& id, const BasicType *type);
    const BasicType* findSymbol(const std::string& id);

    void addTypeDeclaration(const std::string& id);
    const BasicType* findBasicType(const std::string& id);

    Context* createChildContext();
};