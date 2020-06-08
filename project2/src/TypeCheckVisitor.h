#pragma once

#include <memory>
#include "BasicVisitor.h"
#include "Context.h"
#include "Errors.h"

class TypeCheckVisitor : public BasicVisitor
{
private:
  const BasicType *resultExpType;
  const BasicType *returnType;

public:
  // Takes ownership of the context.
  TypeCheckVisitor(Context *context);
  virtual ~TypeCheckVisitor();

  virtual void visitPDefs(PDefs *p) ;
  virtual void visitDFun(DFun *p) ;
  virtual void visitSExp(SExp *p) ;
  virtual void visitSDecls(SDecls *p) ;
  virtual void visitSReturn(SReturn *p) ;
  virtual void visitSReturnV(SReturnV *p) ;
  virtual void visitSWhile(SWhile *p) ;
  virtual void visitSDoWhile(SDoWhile *p) ;
  virtual void visitSFor(SFor *p) ;
  virtual void visitSBlock(SBlock *p) ;
  virtual void visitSIfElse(SIfElse *p) ;
  virtual void visitIdNoInit(IdNoInit *p);
  virtual void visitIdInit(IdInit *p);
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
};