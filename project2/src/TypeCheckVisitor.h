#pragma once

#include <memory>
#include "EmptyVisitor.h"
#include "Context.h"
#include "Errors.h"

class TypeCheckVisitor : public EmptyVisitor
{
private:
  Context *currentContext;
  std::string currentTypeId;
  const BasicType *resultExpType;
  const BasicType *returnType;

  void beginContext();
  void endContext();

  void handleError(const TypeCheckingError &e);
  void tryVisit(Visitable *v);
public:
  TypeCheckVisitor();
  virtual ~TypeCheckVisitor();

  bool anyErrors;

  virtual void visitPDefs(PDefs *p) ;
  virtual void visitDFun(DFun *p) ;
  virtual void visitDStruct(DStruct *p) ;
  virtual void visitSExp(SExp *p) ;
  virtual void visitSDecls(SDecls *p) ;
  virtual void visitSInit(SInit *p) ;
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
  virtual void visitType_bool(Type_bool *p) ;
  virtual void visitType_int(Type_int *p) ;
  virtual void visitType_double(Type_double *p) ;
  virtual void visitType_void(Type_void *p) ;
  virtual void visitTypeId(TypeId *p) ;
};