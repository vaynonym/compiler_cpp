#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_os_ostream.h"

#include "BasicVisitor.h"
#include "CodeGenContext.h"

// If changing any of those, also modify CodeGenVisitor::createTypeObjects
#define INTEGER_WIDTH 64
#define BOOLEAN_WIDTH 1

class CodeGenVisitor : public BasicVisitor {
  private:
    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;

    llvm::Value *expValue;

    std::map<const BasicType *, llvm::Type *> typeMap;

    CodeGenContext *codeGenContext;

    void beginCodeGenContext();
    void endCodeGenContext();

    void createTypeObjects();
  public:
    CodeGenVisitor(Context *context);
    ~CodeGenVisitor();

    void printIR();

    virtual void visitPDefs(PDefs *p);
    virtual void visitDFun(DFun *p);

    virtual void visitSReturn(SReturn *p);

    virtual void visitEInt(EInt *p);
    virtual void visitEDouble(EDouble *p);
    virtual void visitETrue(ETrue *p);
    virtual void visitEFalse(EFalse *p);
    virtual void visitEId(EId *p);

    virtual void visitEEq(EEq *p);
};