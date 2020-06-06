#include "TypeCheckVisitor.h"
#include <iostream>
#include "Errors.h"

#define LOG_LINE std::cout << "Line " << __LINE__ << std::endl;

TypeCheckVisitor::TypeCheckVisitor() {
  currentContext = Context::getDefaultContext();
}

TypeCheckVisitor::~TypeCheckVisitor() {
  delete currentContext;
}

void TypeCheckVisitor::visitPDefs(PDefs *p) {
  p->listdef_->accept(this);
}

void TypeCheckVisitor::visitListDef(ListDef *p) {
  for (auto def : *p) {
      try {
        def->accept(this);
      } catch(TypeCheckingError &e) {
        std::cout << e.printError() << std::endl;
      }
  }
}

void TypeCheckVisitor::visitDFun(DFun *p) {
  // TODO: Return types, arguments, child contexts, etc
  for (auto stm : *p->liststm_) {
    try {
      stm->accept(this);
    } catch (TypeCheckingError &e) {
      std::cout << e.printError() << std::endl;
    }
  }
}

void TypeCheckVisitor::visitDStruct(DStruct *p) {
  LOG_LINE;
}

void TypeCheckVisitor::visitFDecl(FDecl *p) {
  
}

void TypeCheckVisitor::visitADecl(ADecl *p) {
  
}

void TypeCheckVisitor::visitSExp(SExp *p) {
  
}

void TypeCheckVisitor::visitSDecls(SDecls *p) {
  
}

void TypeCheckVisitor::visitSInit(SInit *p) {
  p->type_->accept(this);
  const BasicType *declType = currentContext->findBasicType(currentTypeId);

  if (declType == nullptr) {
    throw UnknownType(currentTypeId);
    return;
  }

  p->exp_->accept(this);

  if (!resultExpType->isConvertibleTo(declType)) {
    throw TypeMismatch(declType->Id, resultExpType->Id, "initialization");
  }

  if(currentContext->findSymbol(p->id_) != nullptr){
    throw IdentifierAlreadyExists(p->id_);
  }
  currentContext->addSymbol(p->id_, declType);
}

void TypeCheckVisitor::visitSReturn(SReturn *p) {
  
}

void TypeCheckVisitor::visitSReturnV(SReturnV *p) {
  
}

void TypeCheckVisitor::visitSWhile(SWhile *p) {
  
}

void TypeCheckVisitor::visitSDoWhile(SDoWhile *p) {
  
}

void TypeCheckVisitor::visitSFor(SFor *p) {
  p->exp_1->accept(this);

  p->exp_2->accept(this);
  if(resultExpType != Context::TYPE_BOOL){
    throw TypeMismatch(Context::TYPE_BOOL->Id, resultExpType->Id, "second expression of for statement");
  }

  p->exp_3->accept(this);

  p->stm_->accept(this);
}

void TypeCheckVisitor::visitSBlock(SBlock *p) {
  
}

void TypeCheckVisitor::visitSIfElse(SIfElse *p) {
  
}

void TypeCheckVisitor::visitETrue(ETrue *p) {
  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEFalse(EFalse *p) {
  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEInt(EInt *p) {
  resultExpType = Context::TYPE_INT;
}

void TypeCheckVisitor::visitEDouble(EDouble *p) {
  resultExpType = Context::TYPE_DOUBLE;
}

void TypeCheckVisitor::visitEId(EId *p) {
  const BasicType *type = currentContext->findSymbol(p->id_);
  if (type == nullptr) {
    throw UnknownType(p->id_);
  }

  resultExpType = type;
}

void TypeCheckVisitor::visitEApp(EApp *p) {
  
}

void TypeCheckVisitor::visitEProj(EProj *p) {
  
}

void TypeCheckVisitor::visitEPIncr(EPIncr *p) {
  
}

void TypeCheckVisitor::visitEPDecr(EPDecr *p) {
  
}

void TypeCheckVisitor::visitEIncr(EIncr *p) {

}

void TypeCheckVisitor::visitEDecr(EDecr *p) {
  
}

void TypeCheckVisitor::visitETimes(ETimes *p) {
  p->exp_1->accept(this);
  if(resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE){
    throw TypeMismatch(Context::TYPE_INT->Id + " or " + Context::TYPE_DOUBLE->Id, resultExpType->Id, "multiplication");
  }
  const BasicType *firstType = resultExpType; 

  p->exp_2->accept(this);
  if( ! resultExpType->isConvertibleTo(firstType) && ! firstType->isConvertibleTo(resultExpType)) {
    throw TypeMismatch(Context::TYPE_INT->Id + " or " + Context::TYPE_DOUBLE->Id, resultExpType->Id, "multiplication");
  }

  if(resultExpType->isConvertibleTo(firstType)) {
    resultExpType = firstType;
  }
}

void TypeCheckVisitor::visitEDiv(EDiv *p) {
  
}

void TypeCheckVisitor::visitEPlus(EPlus *p) {
  
}

void TypeCheckVisitor::visitEMinus(EMinus *p) {
  
}

void TypeCheckVisitor::visitETwc(ETwc *p) {
  p->exp_1->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->Id + " or " + Context::TYPE_DOUBLE->Id, resultExpType->Id);
  }

  p->exp_2->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->Id + " or " + Context::TYPE_DOUBLE->Id, resultExpType->Id);
  }

  resultExpType = Context::TYPE_INT;
}

void TypeCheckVisitor::visitELt(ELt *p) {
  
}

void TypeCheckVisitor::visitEGt(EGt *p) {
  
}

void TypeCheckVisitor::visitELtEq(ELtEq *p) {
  p->exp_1->accept(this);
  if(resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE){
    throw TypeMismatch(Context::TYPE_INT->Id + " or " + Context::TYPE_DOUBLE->Id, resultExpType->Id, " <=");
  }
  const BasicType *firstType = resultExpType; 

  p->exp_2->accept(this);
  if( ! resultExpType->isConvertibleTo(firstType) && ! firstType->isConvertibleTo(resultExpType)){
    throw TypeMismatch(Context::TYPE_INT->Id + " or " + Context::TYPE_DOUBLE->Id, resultExpType->Id, " <=");
  }
  
  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEGtEq(EGtEq *p) {
  
}

void TypeCheckVisitor::visitEEq(EEq *p) {
    
    p->exp_1->accept(this);
    const BasicType *expectedType = resultExpType;

    p->exp_2->accept(this);
    
    if(! resultExpType->isConvertibleTo(expectedType) && ! expectedType->isConvertibleTo(resultExpType)) {
      throw TypeMismatch(expectedType->Id, resultExpType->Id, "== operand");
    }

    resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitENEq(ENEq *p) {
    
    p->exp_1->accept(this);
    const BasicType *expectedType = resultExpType;

    p->exp_2->accept(this);
    
    if( ! resultExpType->isConvertibleTo(expectedType) && ! expectedType->isConvertibleTo(resultExpType)) {
      throw TypeMismatch(expectedType->Id, resultExpType->Id, "!= operand");
    }

    resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEAnd(EAnd *p) {
  p->exp_1->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->Id, resultExpType->Id);
  }

  p->exp_2->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->Id, resultExpType->Id);
  }

  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEOr(EOr *p) {
  p->exp_1->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->Id, resultExpType->Id);
  }

  p->exp_2->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->Id, resultExpType->Id);
  }

  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEAss(EAss *p) {
  p->exp_1->accept(this);
  const BasicType *firstresultExpType = resultExpType;

  p->exp_2->accept(this);
  // checks if right hand side is convertible to type of left hand side
  if (!resultExpType->isConvertibleTo(firstresultExpType)) {
    throw TypeMismatch(firstresultExpType->Id, resultExpType->Id, "= operator");
  }
  // TODO: check if right hand side is initialized
  // sets resultExpType back to type of left hand side
  p->exp_1->accept(this); 
}

void TypeCheckVisitor::visitECond(ECond *p) {
  p->exp_1->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->Id, resultExpType->Id, "condition in the ternary operator");
  } 

  p->exp_2->accept(this);

  const BasicType *firstresultExpType = resultExpType;

  p->exp_3->accept(this);
  if (!resultExpType->isConvertibleTo(firstresultExpType)){
    throw TypeMismatch(firstresultExpType->Id, resultExpType->Id, "third operator not convertible to type of second operator");
  }

  const BasicType *secondresultExpType = resultExpType;
  p->exp_2->accept(this);
  if (!resultExpType->isConvertibleTo(secondresultExpType)){
    throw TypeMismatch(firstresultExpType->Id, resultExpType->Id, "second operator not convertible to type of third operator");
  }

  //TODO: type error in the ternary operator can lead to 2 different resultExpType and may invoke other problems
}

void TypeCheckVisitor::visitType_bool(Type_bool *p) {
  currentTypeId = "bool";
}

void TypeCheckVisitor::visitType_int(Type_int *p) {
  currentTypeId = "int";
}

void TypeCheckVisitor::visitType_double(Type_double *p) {
  currentTypeId = "double";
}

void TypeCheckVisitor::visitType_void(Type_void *p) {
  currentTypeId = "void";
}

void TypeCheckVisitor::visitTypeId(TypeId *p) {
  currentTypeId = p->id_;
}


void TypeCheckVisitor::visitListField(ListField *p) {
  
}

void TypeCheckVisitor::visitListArg(ListArg *p) {
  
}

void TypeCheckVisitor::visitListStm(ListStm *p) {
  
}

void TypeCheckVisitor::visitListExp(ListExp *p) {
  
}

void TypeCheckVisitor::visitListId(ListId *p) {
  
}

void TypeCheckVisitor::visitId(Id x) {
  
}
