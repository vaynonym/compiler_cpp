#include "ContextBuilder.h"


ContextBuilder::ContextBuilder() {
  currentContext = Context::getDefaultContext();
  anyErrors = false;
}

Context *ContextBuilder::getGlobalContext() {
  Context *context = currentContext;
  currentContext = nullptr;
  return context;
}

void ContextBuilder::visitPDefs(PDefs *p) {
  for (auto def : *p->listdef_) {
    tryVisit(def);
  }
}

void ContextBuilder::visitDFun(DFun *p) {
  std::vector<const BasicType *> argumentTypes;
  for (auto arg : *p->listarg_) {
    ADecl *adecl = (ADecl *) arg;

    adecl->type_->accept(this);
    const BasicType *argType = currentContext->findBasicType(currentTypeId);
    if( argType == Context::TYPE_VOID){
      handleError(TypeMismatch("non-void Type", Context::TYPE_VOID->id, "function arguments can not be of type void"));
      continue;
    }
    if (argType == nullptr) {
      handleError(UnknownType(currentTypeId, "function argument"));
      continue;
    }

    argumentTypes.push_back(argType);
  }

  p->type_->accept(this);
  const BasicType *returnType = currentContext->findBasicType(currentTypeId);
  if (returnType == nullptr) {
    handleError(UnknownType(currentTypeId, "function return type"));
  }
  else {
    if (!currentContext->addFunction(p->id_, new FunctionType(returnType, argumentTypes))) {
      throw IdentifierAlreadyExists(p->id_, "function declaration");
    }
  }
}

void ContextBuilder::visitDStruct(DStruct *p) {
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

