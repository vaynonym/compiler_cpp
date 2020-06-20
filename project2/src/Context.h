#pragma once

#include <string>
#include <map>
#include <vector>
#include "Types.h"

class Context {
  private:
    std::vector<const BasicType *> types;
    std::map<const std::string, const BasicType *> basicSymbols;
    std::map<const std::string, const FunctionType *> functionSymbols;

    Context() {}
    const BasicType *findSymbolInThis(const std::string& id);
    const FunctionType *findFunctionInThis(const std::string& id);

  public:
    static const BasicType *TYPE_INT;
    static const BasicType *TYPE_DOUBLE;
    static const BasicType *TYPE_BOOL;
    static const BasicType *TYPE_VOID;

    // Must only ever be called once during the program's lifetime, because deleting the default
    // context also deletes the basic primitive types.
    static Context* getDefaultContext();

    Context* parent;

    ~Context();

    bool addSymbol(const std::string& id, const BasicType *type);
    const BasicType* findSymbol(const std::string& id);

    bool addTypeDeclaration(const std::string& id);
    bool addStructDeclaration(const std::string& id, const std::map<const std::string, const BasicType *> members);
    const BasicType* findBasicType(const std::string& id);

    bool addFunction(const std::string& id, const FunctionType *type);
    const FunctionType* findFunction(const std::string& id);

    const std::vector<const BasicType *> getKnownTypes() const;

    Context* createChildContext();
};