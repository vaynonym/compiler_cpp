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
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/raw_os_ostream.h"

#include "BasicVisitor.h"
#include "CodeGenContext.h"

// If changing any of those, also modify CodeGenVisitor::createTypeObjects
#define INTEGER_WIDTH 64
#define BOOLEAN_WIDTH 1

#define TO_INT_VALUE(x) (llvm::APInt(INTEGER_WIDTH, x, true))
#define TO_DOUBLE_VALUE(x) (llvm::APFloat(x))
#define TO_BOOL_VALUE(x) (llvm::APInt(BOOLEAN_WIDTH, x ? 1 : 0, false))

#define UNREACHABLE_OH_NO throw "oh no 😱";

class CodeGenVisitor : public BasicVisitor {
  private:
    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;

    const FunctionType *currentFunction;
    llvm::Value *expValue;

    std::map<const BasicType *, llvm::Type *> typeMap;

    CodeGenContext *codeGenContext;

    void beginCodeGenContext();
    void endCodeGenContext();

    void createTypeObjects();
  public:
    CodeGenVisitor(const char *moduleName, Context *context);
    ~CodeGenVisitor();

    void printIR();

    virtual void visitPDefs(PDefs *p);
    virtual void visitDFun(DFun *p);

    virtual void visitSExp(SExp *p) ;
    virtual void visitSDecls(SDecls *p) ;
    virtual void visitSReturn(SReturn *p) ;
    virtual void visitSReturnV(SReturnV *p) ;
    virtual void visitSWhile(SWhile *p) ;
    virtual void visitSDoWhile(SDoWhile *p) ;
    virtual void visitSFor(SFor *p) ;
    virtual void visitSBlock(SBlock *p) ;
    virtual void visitSIfElse(SIfElse *p) ;
    virtual void visitETrue(ETrue *p) ;
    virtual void visitEFalse(EFalse *p) ;
    virtual void visitEInt(EInt *p) ;
    virtual void visitEDouble(EDouble *p) ;
    virtual void visitEId(EId *p) ;
    virtual void visitEApp(EApp *p) ;
    virtual void visitEProj(EProj *p) ;
    virtual void visitEPIncr(EPIncr *p) ;
    virtual void visitEPDecr(EPDecr *p) ;
    virtual void visitEIncr(EIncr *p) ;
    virtual void visitEDecr(EDecr *p) ;
    virtual void visitEUPlus(EUPlus *p);
    virtual void visitEUMinus(EUMinus *p);
    virtual void visitETimes(ETimes *p) ;
    virtual void visitEDiv(EDiv *p) ;
    virtual void visitEPlus(EPlus *p) ;
    virtual void visitEMinus(EMinus *p) ;
    virtual void visitETwc(ETwc *p) ;
    virtual void visitELt(ELt *p) ;
    virtual void visitEGt(EGt *p) ;
    virtual void visitELtEq(ELtEq *p) ;
    virtual void visitEGtEq(EGtEq *p) ;
    virtual void visitEEq(EEq *p) ;
    virtual void visitENEq(ENEq *p) ;
    virtual void visitEAnd(EAnd *p) ;
    virtual void visitEOr(EOr *p) ;
    virtual void visitEAss(EAss *p) ;
    virtual void visitECond(ECond *p) ;

    llvm::Value *genEqComparison(const BasicType *firstType, llvm::Value *firstVal, const BasicType *secondType, llvm::Value *secondVal);
    llvm::Value *genStructAccess(llvm::Value *structVal, int memberIndex);
    llvm::Value *genAssignment(Exp *e1, llvm::Value *assignedValue);

    int getMemberIndex(llvm::StructType *llvmSType, std::string& memberName);

    bool handleNumberConversions(const BasicType *firstType, llvm::Value **firstVal,
      const BasicType *secondType, llvm::Value **secondVal,
      const BasicType **resultType);
};