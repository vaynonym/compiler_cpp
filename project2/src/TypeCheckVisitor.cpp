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
    // TODO: Error handling
    return;
  }

  p->exp_->accept(this);

  if (!resultExpType->isConvertibleTo(declType)) {
    // TODO: Error handling
    std::cout << "Type mismatch" << std::endl;
    return;
  }

  // TODO: Check if name already exists
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
    // TODO: throw error
  }

  p->exp_2->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    // TODO: throw error
  }

  resultExpType = Context::TYPE_INT;
}

void TypeCheckVisitor::visitELt(ELt *p) {
  
}

void TypeCheckVisitor::visitEGt(EGt *p) {
  
}

void TypeCheckVisitor::visitELtEq(ELtEq *p) {
  
}

void TypeCheckVisitor::visitEGtEq(EGtEq *p) {
  
}

void TypeCheckVisitor::visitEEq(EEq *p) {
    
    p->exp_1->accept(this);
    const BasicType *expectedType = resultExpType;

    p->exp_2->accept(this);
    
    if(! resultExpType->isConvertibleTo(expectedType)) {
      throw TypeMismatch(expectedType->Id, resultExpType->Id, "== operand");
    }

    resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitENEq(ENEq *p) {
  
}

void TypeCheckVisitor::visitEAnd(EAnd *p) {
  p->exp_1->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    // TODO: throw type mismatch error
    std::cout << "Type Error!" << std::endl;
  }

  p->exp_2->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    // TODO: throw type mismatch error
    std::cout << "Type Error!" << std::endl;
  }

  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEOr(EOr *p) {
  
}

void TypeCheckVisitor::visitEAss(EAss *p) {
  
}

void TypeCheckVisitor::visitECond(ECond *p) {
  
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
