#pragma once

#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "BasicVisitor.h"
#include "Context.h"

class LLVMContextBuilder : public BasicVisitor {
  private:
    Context *context;
    llvm::LLVMContext *llvmContext;
    llvm::IRBuilder<> *builder;
    llvm::Module *module;

    std::map<const BasicType *, llvm::Type *> *typeMap;

    void createTypeObjects();
  public:

    LLVMContextBuilder(Context *context, llvm::LLVMContext *llvmContext, llvm::IRBuilder<> *builder,
        std::map<const BasicType *, llvm::Type *> *typeMap, llvm::Module *module);

    virtual void visitPDefs(PDefs *p);
    virtual void visitDFun(DFun *p);
};