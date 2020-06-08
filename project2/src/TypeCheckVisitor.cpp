#include "TypeCheckVisitor.h"
#include <iostream>
#include "Errors.h"

#define LOG_LINE std::cout << "Line " << __LINE__ << std::endl;

TypeCheckVisitor::TypeCheckVisitor() {
  currentContext = Context::getDefaultContext();
  anyErrors = false;
}

TypeCheckVisitor::~TypeCheckVisitor() {
  delete currentContext;
}

void TypeCheckVisitor::beginContext() {
  currentContext = currentContext->createChildContext();
}

void TypeCheckVisitor::endContext() {
  Context *oldContext = currentContext;
  currentContext = currentContext->parent;
  delete oldContext;
}

void TypeCheckVisitor::handleError(const TypeCheckingError &e) {
  std::cout << "TYPE ERROR" << std::endl << e.printError() << std::endl << std::endl;
  anyErrors = true;
}

void TypeCheckVisitor::tryVisit(Visitable *v) {
  try {
    v->accept(this);
  } catch (const TypeCheckingError &e) {
    handleError(e);
  }
}

void TypeCheckVisitor::visitPDefs(PDefs *p) {
  for (auto def : *p->listdef_) {
    tryVisit(def);
  }
}

void TypeCheckVisitor::visitDFun(DFun *p) {
  beginContext();

  std::vector<const BasicType *> argumentTypes;
  for (auto arg : *p->listarg_) {
    ADecl *adecl = (ADecl *) arg;

    adecl->type_->accept(this);
    const BasicType *argType = currentContext->findBasicType(currentTypeId);
    if (argType == nullptr) {
      handleError(UnknownType(currentTypeId, "function argument"));
      continue;
    }

    if (!currentContext->addSymbol(adecl->id_, argType)) {
      handleError(IdentifierAlreadyExists(adecl->id_));
      continue;
    }

    argumentTypes.push_back(argType);
  }

  p->type_->accept(this);
  const BasicType *returnType = currentContext->findBasicType(currentTypeId);
  if (returnType == nullptr) {
    handleError(UnknownType(currentTypeId, "function return type"));
  }

  this->returnType = returnType;

  for (auto stm : *p->liststm_) {
    tryVisit(stm);
  }

  endContext();

  if (returnType != nullptr) {
    if (!currentContext->addFunction(p->id_, new FunctionType(returnType, argumentTypes))) {
      throw IdentifierAlreadyExists(p->id_, "function declaration");
    }
  }
}

void TypeCheckVisitor::visitDStruct(DStruct *p) {
  std::map<const std::string, const BasicType *> members;

  for (auto *field : *p->listfield_) {
    FDecl *fdecl = (FDecl *) field;

    if (members.find(fdecl->id_) != members.end()) {
      handleError(IdentifierAlreadyExists(fdecl->id_));
      continue;
    }

    fdecl->type_->accept(this);
    const BasicType *memberType = currentContext->findBasicType(currentTypeId);
    if (memberType == nullptr) {
      handleError(UnknownType(currentTypeId, "struct member"));
      continue;
    }

    members.emplace(fdecl->id_, memberType);
  }

  bool success = currentContext->addStructDeclaration(p->id_, members);
  if (!success) throw TypeAlreadyExists(p->id_);
}

void TypeCheckVisitor::visitSExp(SExp *p) {
  p->exp_->accept(this);
}

void TypeCheckVisitor::visitSDecls(SDecls *p) {
  p->type_->accept(this);
  const BasicType *declType = currentContext->findBasicType(currentTypeId);

  if (declType == nullptr) {
    throw UnknownType(currentTypeId);
    return;
  }
  
  for (IdIn id : *(p->listidin_)){
    if(currentContext->findSymbol(id) != nullptr){
      throw IdentifierAlreadyExists(id);
    }
    currentContext->addSymbol(id, declType);
  }

}
/*
void TypeCheckVisitor::visitSInit(SInit *p) {
  p->type_->accept(this);
  const BasicType *declType = currentContext->findBasicType(currentTypeId);

  if (declType == nullptr) {
    throw UnknownType(currentTypeId);
    return;
  }

  p->exp_->accept(this);

  if (!resultExpType->isConvertibleTo(declType)) {
    throw TypeMismatch(declType->id, resultExpType->id, "initialization");
  }

  if(currentContext->findSymbol(p->id_) != nullptr){
    throw IdentifierAlreadyExists(p->id_);
  }
  currentContext->addSymbol(p->id_, declType);
}
*/
void TypeCheckVisitor::visitSReturn(SReturn *p) {
  p->exp_->accept(this);
  if (!resultExpType->isConvertibleTo(returnType)) {
    throw TypeMismatch(returnType->id, resultExpType->id, "return type");
  }
}

void TypeCheckVisitor::visitSReturnV(SReturnV *p) {
  if (!returnType->isConvertibleTo(Context::TYPE_VOID)) {
    throw TypeMismatch(returnType->id, Context::TYPE_VOID->id, "return type");
  }
}

void TypeCheckVisitor::visitSWhile(SWhile *p) {
  p->exp_->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->id, resultExpType->id, "while condition");
  }

  p->stm_->accept(this);
}

void TypeCheckVisitor::visitSDoWhile(SDoWhile *p) {
  p->stm_->accept(this);

  p->exp_->accept(this);
  if(resultExpType != Context::TYPE_BOOL){
    throw TypeMismatch(Context::TYPE_BOOL->id, resultExpType->id, "Do while statement, condition expression");
  }
}

void TypeCheckVisitor::visitSFor(SFor *p) {
  p->exp_1->accept(this);

  p->exp_2->accept(this);
  if(resultExpType != Context::TYPE_BOOL){
    throw TypeMismatch(Context::TYPE_BOOL->id, resultExpType->id, "second expression of for statement");
  }

  p->exp_3->accept(this);

  p->stm_->accept(this);
}

void TypeCheckVisitor::visitSBlock(SBlock *p) {
  beginContext();

  for (auto stm : *p->liststm_) {
    tryVisit(stm);
  }

  endContext();
}

void TypeCheckVisitor::visitSIfElse(SIfElse *p) {
  p->exp_->accept(this);
  if(resultExpType != Context::TYPE_BOOL){
    throw TypeMismatch(Context::TYPE_BOOL->id, resultExpType->id, "IfElse statement, conditional expression");
  }
  p->stm_1->accept(this);
  p->stm_2->accept(this);
}
void TypeCheckVisitor::visitIdNoInit(IdNoInit *p){

}
void TypeCheckVisitor::visitIdInit(IdInit *p){
  
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
  const FunctionType *function = currentContext->findFunction(p->id_);
  if (function == nullptr) {
    throw UnknownIdentifier(p->id_);
  }

  bool hasError = function->parameters.size() != p->listexp_->size();
  std::string actualSig = p->id_ + "(";
  for (size_t i = 0; i < p->listexp_->size(); i++) {
    (*p->listexp_)[i]->accept(this);

    if (i < function->parameters.size()) {
      hasError |= !resultExpType->isConvertibleTo(function->parameters[i]);
    }

    actualSig += resultExpType->id;
    if (i != p->listexp_->size() - 1)
      actualSig += ", ";
  }
  actualSig += ")";

  if (hasError) {
    std::string functionSig = p->id_ + "(";
    for (size_t j = 0; j < function->parameters.size(); j++) {
      functionSig += function->parameters[j]->id;
      if (j != function->parameters.size() - 1)
        functionSig += ", ";
    }
    functionSig += ")";

    throw FunctionSignatureMismatch(functionSig, actualSig, p->id_);
  }

  resultExpType = function->returnType;
}

void TypeCheckVisitor::visitEProj(EProj *p) {
  p->exp_->accept(this);

  const BasicType *memberType = resultExpType->getMember(p->id_);
  if (memberType == nullptr) throw UnknownIdentifier(p->id_, "struct member");

  resultExpType = memberType;
}

void TypeCheckVisitor::visitEPIncr(EPIncr *p) {
  p->exp_->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id);
  }
} 

void TypeCheckVisitor::visitEPDecr(EPDecr *p) {
  p->exp_->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id);
  }
}

void TypeCheckVisitor::visitEIncr(EIncr *p) {
  p->exp_->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id);
  }
}

void TypeCheckVisitor::visitEDecr(EDecr *p) {
  p->exp_->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id);
  }
}

void TypeCheckVisitor::visitEUPlus(EUPlus *p) {
  p->exp_->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id);
  }
}
void TypeCheckVisitor::visitEUMinus(EUMinus *p) {
  p->exp_->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id);
  }
}


void TypeCheckVisitor::visitETimes(ETimes *p) {
  p->exp_1->accept(this);
  if(resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE){
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, "multiplication");
  }
  const BasicType *firstType = resultExpType; 

  p->exp_2->accept(this);
  if( ! resultExpType->isConvertibleTo(firstType) && ! firstType->isConvertibleTo(resultExpType)) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, "multiplication");
  }

  if(resultExpType->isConvertibleTo(firstType)) {
    resultExpType = firstType;
  }
}

void TypeCheckVisitor::visitEDiv(EDiv *p) {
  p->exp_1->accept(this);
  if(resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE){
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, "division");
  }
  const BasicType *firstType = resultExpType; 

  p->exp_2->accept(this);
  if( ! resultExpType->isConvertibleTo(firstType) && ! firstType->isConvertibleTo(resultExpType)) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, "division");
  }

  if(firstType==Context::TYPE_INT&&resultExpType==Context::TYPE_INT){
    resultExpType = firstType;
  }
  else{
    resultExpType = Context::TYPE_DOUBLE;
  }
}

void TypeCheckVisitor::visitEPlus(EPlus *p) {
  p->exp_1->accept(this);
  if(!(resultExpType == Context::TYPE_DOUBLE || resultExpType == Context::TYPE_INT)){
    throw TypeMismatch(Context::TYPE_INT->id + "or" + Context::TYPE_DOUBLE->id, resultExpType->id, "Addition, first operand");
  }

  const BasicType* firstresultExpType = resultExpType;

  p->exp_2->accept(this);
  if(!(resultExpType == Context::TYPE_DOUBLE || resultExpType == Context::TYPE_INT)){
    throw TypeMismatch(Context::TYPE_INT->id + "or" + Context::TYPE_DOUBLE->id, resultExpType->id, "Addition, second operand");
  }
  // in this simple grammar (only int and double as numeric types) this checks for 'int + double' and 'double + int', in both cases we have
  // to convert int to double 
  // technically, this should throw a warning
  if(resultExpType != firstresultExpType){
    resultExpType = Context::TYPE_DOUBLE;
  }
}

void TypeCheckVisitor::visitEMinus(EMinus *p) {
  p->exp_1->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, "-");
  }
  const BasicType *first = resultExpType;

  p->exp_2->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, "-");
  }
  const BasicType *second = resultExpType;

  bool anyDouble = first == Context::TYPE_DOUBLE || second == Context::TYPE_DOUBLE;

  resultExpType = anyDouble ? Context::TYPE_DOUBLE : Context::TYPE_INT;
}

void TypeCheckVisitor::visitETwc(ETwc *p) {
  p->exp_1->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id);
  }

  p->exp_2->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id);
  }

  resultExpType = Context::TYPE_INT;
}

void TypeCheckVisitor::visitELt(ELt *p) {

  p->exp_1->accept(this);
  if(resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE){
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, "Less than operator, first operand");
  }

  p->exp_2->accept(this);
  if(resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE){
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, "Less than operator, second operand");
  }

  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEGt(EGt *p) {
  p->exp_1->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, " >=");
  }

  p->exp_2->accept(this);
  if (resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE) {
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, " >=");
  }

  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitELtEq(ELtEq *p) {
  p->exp_1->accept(this);
  if(resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE){
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, " <=");
  }
  const BasicType *firstType = resultExpType; 

  p->exp_2->accept(this);
  if( ! resultExpType->isConvertibleTo(firstType) && ! firstType->isConvertibleTo(resultExpType)){
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, " <=");
  }
  
  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEGtEq(EGtEq *p) {
  p->exp_1->accept(this);
  if(resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE){
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, " >=");
  }
  p->exp_2->accept(this);
  if(resultExpType != Context::TYPE_INT && resultExpType != Context::TYPE_DOUBLE){
    throw TypeMismatch(Context::TYPE_INT->id + " or " + Context::TYPE_DOUBLE->id, resultExpType->id, " >=");
  }
  
  resultExpType = Context::TYPE_BOOL;
  
}

void TypeCheckVisitor::visitEEq(EEq *p) {
  p->exp_1->accept(this);
  const BasicType *expectedType = resultExpType;

  p->exp_2->accept(this);
  
  if(! resultExpType->isConvertibleTo(expectedType) && ! expectedType->isConvertibleTo(resultExpType)) {
    throw TypeMismatch(expectedType->id, resultExpType->id, "== operand");
  }

  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitENEq(ENEq *p) {
  p->exp_1->accept(this);
  const BasicType *expectedType = resultExpType;

  p->exp_2->accept(this);
  
  if( ! resultExpType->isConvertibleTo(expectedType) && ! expectedType->isConvertibleTo(resultExpType)) {
    throw TypeMismatch(expectedType->id, resultExpType->id, "!= operand");
  }

  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEAnd(EAnd *p) {
  p->exp_1->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->id, resultExpType->id);
  }

  p->exp_2->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->id, resultExpType->id);
  }

  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEOr(EOr *p) {
  p->exp_1->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->id, resultExpType->id);
  }

  p->exp_2->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->id, resultExpType->id);
  }

  resultExpType = Context::TYPE_BOOL;
}

void TypeCheckVisitor::visitEAss(EAss *p) {
  p->exp_1->accept(this);
  const BasicType *firstresultExpType = resultExpType;
  
  p->exp_2->accept(this);
  // checks if right hand side is convertible to type of left hand side
  if (!resultExpType->isConvertibleTo(firstresultExpType)) {
    throw TypeMismatch(firstresultExpType->id, resultExpType->id, "= operator");
  }
  // sets resultExpType back to type of left hand side
  resultExpType = firstresultExpType;
}

void TypeCheckVisitor::visitECond(ECond *p) {
  p->exp_1->accept(this);
  if (!resultExpType->isConvertibleTo(Context::TYPE_BOOL)) {
    throw TypeMismatch(Context::TYPE_BOOL->id, resultExpType->id, "condition in the ternary operator");
  } 

  p->exp_2->accept(this);

  const BasicType *firstresultExpType = resultExpType;

  p->exp_3->accept(this);
  if (resultExpType != firstresultExpType){
    throw TypeMismatch(firstresultExpType->id, resultExpType->id, "second and third operand");
  }
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