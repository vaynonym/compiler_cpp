#include "BasicVisitor.h"
#include <iostream>

void BasicVisitor::beginContext() {
  currentContext = currentContext->createChildContext();
}

void BasicVisitor::endContext() {
  Context *oldContext = currentContext;
  currentContext = currentContext->parent;
  delete oldContext;
}

void BasicVisitor::handleError(const TypeCheckingError &e) {
  std::cout << "TYPE ERROR" << std::endl << e.printError() << std::endl << std::endl;
  anyErrors = true;
}

void BasicVisitor::tryVisit(Visitable *v) {
  try {
    v->accept(this);
  } catch (const TypeCheckingError &e) {
    handleError(e);
  }
}

void BasicVisitor::visitType_bool(Type_bool *p) {
  currentTypeId = "bool";
}

void BasicVisitor::visitType_int(Type_int *p) {
  currentTypeId = "int";
}

void BasicVisitor::visitType_double(Type_double *p) {
  currentTypeId = "double";
}

void BasicVisitor::visitType_void(Type_void *p) {
  currentTypeId = "void";
}

void BasicVisitor::visitTypeId(TypeId *p) {
  currentTypeId = p->id_;
}

void BasicVisitor::visitProgram(Program *p) {
    
}

void BasicVisitor::visitDef(Def *p) {
    
}

void BasicVisitor::visitField(Field *p) {
    
}

void BasicVisitor::visitArg(Arg *p) {
    
}

void BasicVisitor::visitStm(Stm *p) {
    
}

void BasicVisitor::visitIdIn(IdIn *p) {

}

void BasicVisitor::visitExp(Exp *p) {
    
}

void BasicVisitor::visitType(Type *p) {
    
}

void BasicVisitor::visitPDefs(PDefs *p) {
    
}

void BasicVisitor::visitDFun(DFun *p) {
    
}

void BasicVisitor::visitDStruct(DStruct *p) {
    
}

void BasicVisitor::visitFDecl(FDecl *p) {
    
}

void BasicVisitor::visitADecl(ADecl *p) {
    
}

void BasicVisitor::visitSExp(SExp *p) {
    
}

void BasicVisitor::visitSDecls(SDecls *p) {
    
}

void BasicVisitor::visitSReturn(SReturn *p) {
    
}

void BasicVisitor::visitSReturnV(SReturnV *p) {
    
}

void BasicVisitor::visitSWhile(SWhile *p) {
    
}

void BasicVisitor::visitSDoWhile(SDoWhile *p) {
    
}

void BasicVisitor::visitSFor(SFor *p) {
    
}

void BasicVisitor::visitSBlock(SBlock *p) {
    
}

void BasicVisitor::visitSIfElse(SIfElse *p) {
    
}

void BasicVisitor::visitIdNoInit(IdNoInit *p){

}

void BasicVisitor::visitIdInit(IdInit *p){

}

void BasicVisitor::visitETrue(ETrue *p) {
    
}

void BasicVisitor::visitEFalse(EFalse *p) {
    
}

void BasicVisitor::visitEInt(EInt *p) {
    
}

void BasicVisitor::visitEDouble(EDouble *p) {
    
}

void BasicVisitor::visitEId(EId *p) {
    
}

void BasicVisitor::visitEApp(EApp *p) {
    
}

void BasicVisitor::visitEProj(EProj *p) {
    
}

void BasicVisitor::visitEPIncr(EPIncr *p) {
    
}

void BasicVisitor::visitEPDecr(EPDecr *p) {
    
}

void BasicVisitor::visitEIncr(EIncr *p) {
    
}

void BasicVisitor::visitEDecr(EDecr *p) {
    
}
void BasicVisitor::visitEUPlus(EUPlus *p){

}
void BasicVisitor::visitEUMinus(EUMinus *p){

}

void BasicVisitor::visitETimes(ETimes *p) {
    
}

void BasicVisitor::visitEDiv(EDiv *p) {
    
}

void BasicVisitor::visitEPlus(EPlus *p) {
    
}

void BasicVisitor::visitEMinus(EMinus *p) {
    
}

void BasicVisitor::visitETwc(ETwc *p) {
    
}

void BasicVisitor::visitELt(ELt *p) {
    
}

void BasicVisitor::visitEGt(EGt *p) {
    
}

void BasicVisitor::visitELtEq(ELtEq *p) {
    
}

void BasicVisitor::visitEGtEq(EGtEq *p) {
    
}

void BasicVisitor::visitEEq(EEq *p) {
    
}

void BasicVisitor::visitENEq(ENEq *p) {
    
}

void BasicVisitor::visitEAnd(EAnd *p) {
    
}

void BasicVisitor::visitEOr(EOr *p) {
    
}

void BasicVisitor::visitEAss(EAss *p) {
    
}

void BasicVisitor::visitECond(ECond *p) {
    
}

void BasicVisitor::visitListDef(ListDef *p) {
    
}

void BasicVisitor::visitListField(ListField *p) {
    
}

void BasicVisitor::visitListArg(ListArg *p) {
    
}

void BasicVisitor::visitListStm(ListStm *p) {
    
}

void BasicVisitor::visitListIdIn(ListIdIn *p) {

}

void BasicVisitor::visitListExp(ListExp *p) {
    
}

void BasicVisitor::visitListId(ListId *p) {
    
}

void BasicVisitor::visitInteger(Integer x) {
    
}

void BasicVisitor::visitChar(Char x) {
    
}

void BasicVisitor::visitDouble(Double x) {
    
}

void BasicVisitor::visitString(String x) {
    
}

void BasicVisitor::visitIdent(Ident x) {
    
}

void BasicVisitor::visitId(Id x) {
    
}
