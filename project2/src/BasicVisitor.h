#pragma once

#include "Absyn.H"
#include "Context.h"
#include "Errors.h"

class BasicVisitor : public Visitor {
protected:
  Context *currentContext;
  std::string currentTypeId;

  void beginContext();
  void endContext();

  void handleError(const TypeCheckingError &e);
  void tryVisit(Visitable *v);

public:
  bool anyErrors;

  virtual ~BasicVisitor() {}
  virtual void visitProgram(Program *p);
  virtual void visitDef(Def *p);
  virtual void visitField(Field *p);
  virtual void visitArg(Arg *p);
  virtual void visitStm(Stm *p);
  virtual void visitIdIn(IdIn *p);
  virtual void visitExp(Exp *p);
  virtual void visitType(Type *p);
  virtual void visitPDefs(PDefs *p);
  virtual void visitDFun(DFun *p);
  virtual void visitDStruct(DStruct *p);
  virtual void visitFDecl(FDecl *p);
  virtual void visitADecl(ADecl *p);
  virtual void visitSExp(SExp *p);
  virtual void visitSDecls(SDecls *p);
  virtual void visitSReturn(SReturn *p);
  virtual void visitSReturnV(SReturnV *p);
  virtual void visitSWhile(SWhile *p);
  virtual void visitSDoWhile(SDoWhile *p);
  virtual void visitSFor(SFor *p);
  virtual void visitSBlock(SBlock *p);
  virtual void visitSIfElse(SIfElse *p);
  virtual void visitIdNoInit(IdNoInit *p);
  virtual void visitIdInit(IdInit *p);
  virtual void visitETrue(ETrue *p);
  virtual void visitEFalse(EFalse *p);
  virtual void visitEInt(EInt *p);
  virtual void visitEDouble(EDouble *p);
  virtual void visitEId(EId *p);
  virtual void visitEApp(EApp *p);
  virtual void visitEProj(EProj *p);
  virtual void visitEPIncr(EPIncr *p);
  virtual void visitEPDecr(EPDecr *p);
  virtual void visitEIncr(EIncr *p);
  virtual void visitEDecr(EDecr *p);
  virtual void visitEUPlus(EUPlus *p);
  virtual void visitEUMinus(EUMinus *p);
  virtual void visitETimes(ETimes *p);
  virtual void visitEDiv(EDiv *p);
  virtual void visitEPlus(EPlus *p);
  virtual void visitEMinus(EMinus *p);
  virtual void visitETwc(ETwc *p);
  virtual void visitELt(ELt *p);
  virtual void visitEGt(EGt *p);
  virtual void visitELtEq(ELtEq *p);
  virtual void visitEGtEq(EGtEq *p);
  virtual void visitEEq(EEq *p);
  virtual void visitENEq(ENEq *p);
  virtual void visitEAnd(EAnd *p);
  virtual void visitEOr(EOr *p);
  virtual void visitEAss(EAss *p);
  virtual void visitECond(ECond *p);
  virtual void visitType_bool(Type_bool *p);
  virtual void visitType_int(Type_int *p);
  virtual void visitType_double(Type_double *p);
  virtual void visitType_void(Type_void *p);
  virtual void visitTypeId(TypeId *p);
  virtual void visitListDef(ListDef *p);
  virtual void visitListField(ListField *p);
  virtual void visitListArg(ListArg *p);
  virtual void visitListStm(ListStm *p);
  virtual void visitListIdIn(ListIdIn *p);
  virtual void visitListExp(ListExp *p);
  virtual void visitListId(ListId *p);

  virtual void visitInteger(Integer x);
  virtual void visitChar(Char x);
  virtual void visitDouble(Double x);
  virtual void visitString(String x);
  virtual void visitIdent(Ident x);
  virtual void visitId(Id x);
};