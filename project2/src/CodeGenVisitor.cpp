#include "CodeGenVisitor.h"

CodeGenVisitor::CodeGenVisitor(const char *moduleName, Context *context) : llvmContext(), builder(llvmContext) {
  currentContext = context;

  module = llvm::make_unique<llvm::Module>(moduleName, llvmContext);

  createTypeObjects();

  codeGenContext = new CodeGenContext();
}

CodeGenVisitor::~CodeGenVisitor() {
  delete codeGenContext;
}

void CodeGenVisitor::beginCodeGenContext() {
  codeGenContext = codeGenContext->createChildContext();
}

void CodeGenVisitor::endCodeGenContext() {
  CodeGenContext *oldContext = codeGenContext;
  codeGenContext = codeGenContext->parent;
  delete oldContext;
}

void CodeGenVisitor::createTypeObjects() {
  // Go through all types in context:
  //   Create llvm::TYpe object for it
  //   Insert into Type->llvm:Type map

  const std::vector<const BasicType *> knownTypes = currentContext->getKnownTypes();
  for (const BasicType *type : knownTypes) {
    llvm::Type *llvmType;
    if (type == Context::TYPE_INT) {
      llvmType = llvm::Type::getInt64Ty(llvmContext);
    }
    else if (type == Context::TYPE_DOUBLE) {
      llvmType = llvm::Type::getDoubleTy(llvmContext);
    }
    else if (type == Context::TYPE_BOOL) {
      llvmType = llvm::Type::getInt1Ty(llvmContext);
    }
    else if (type == Context::TYPE_VOID) {
      llvmType = llvm::Type::getVoidTy(llvmContext);
    }
    else {
      const StructType *sType = (const StructType *) type;
      auto members = sType->getMemberTypes();

      std::vector<llvm::Type *> memberTypes;
      for (auto type : members) {
        memberTypes.push_back(typeMap[type]);
      }

      llvm::StructType *llvmSType = llvm::StructType::get(llvmContext, memberTypes);
      llvmSType->setName(sType->id);

      llvmType = llvmSType;
    }

    typeMap.emplace(type, llvmType);
  }
}

void CodeGenVisitor::printIR() {
  module->print(llvm::outs(), nullptr);
}

void CodeGenVisitor::visitPDefs(PDefs *p) {
  for (auto def : *p->listdef_) {
    def->accept(this);
  }
}

void CodeGenVisitor::visitDFun(DFun *p) {
  const FunctionType *ft = currentContext->findFunction(p->id_);

  // Map the typechecker's BasicType objects to corresponding llvm::Type objects
  std::vector<llvm::Type *> arguments;
  for (const BasicType *argType : ft->parameters) {
    arguments.push_back(typeMap[argType]);
  }

  llvm::Type *returnType = typeMap[ft->returnType];

  // Create LLVM FunctionType
  llvm::FunctionType *llvmFt = llvm::FunctionType::get(returnType, arguments, false);

  // Create LLVM Function
  llvm::Function *llvmF = llvm::Function::Create(llvmFt, llvm::Function::ExternalLinkage, p->id_, module.get());

  // Set argument names
  int i = 0;
  for (auto& arg : llvmF->args()) {
    arg.setName(((ADecl *) (*p->listarg_)[i++])->id_);
  }

  // Function body

  beginCodeGenContext();
  currentFunction = ft;

  // Codegen statements for function body

  llvm::BasicBlock *bb = llvm::BasicBlock::Create(llvmContext, "entry", llvmF);
  builder.SetInsertPoint(bb);

  // Insert arguments into new child context
  for (auto& arg : llvmF->args()) {
    llvm::Value *ptr = builder.CreateAlloca(arg.getType(), llvm::ConstantInt::get(llvmContext, TO_INT_VALUE(1)));
    builder.CreateStore(&arg, ptr);
    codeGenContext->addSymbol(arg.getName(), ptr);
  }

  for (auto stm : *p->liststm_) {
    stm->accept(this);
  }

  if (ft->returnType == Context::TYPE_VOID) {
    builder.CreateRetVoid();
  }

  endCodeGenContext();

  llvm::verifyFunction(*llvmF, &llvm::outs());
}

void CodeGenVisitor::visitSExp(SExp *p) {
  p->exp_->accept(this);
}

void CodeGenVisitor::visitSDecls(SDecls *p) {
  p->type_->accept(this);
  const BasicType *declType = currentContext->findBasicType(currentTypeId);

  llvm::Type *type = typeMap[declType];

  for (auto idOrInit : *p->listidin_){
    IdInit *init = dynamic_cast<IdInit *>(idOrInit);
    IdNoInit *noInit = dynamic_cast<IdNoInit *>(idOrInit);

    std::string id = init != nullptr ? init->id_ : noInit->id_;

    llvm::Value *ptr = builder.CreateAlloca(type, llvm::ConstantInt::get(llvmContext, TO_INT_VALUE(1)));
    codeGenContext->addSymbol(id, ptr);

    if (init != nullptr) {
      init->exp_->accept(this);
      
      builder.CreateStore(expValue, codeGenContext->findSymbol(id));
    }
  }
}

void CodeGenVisitor::visitSReturn(SReturn *p) {
  p->exp_->accept(this);

  if (currentFunction->returnType == Context::TYPE_DOUBLE && p->exp_->type == Context::TYPE_INT) {
    expValue = builder.CreateSIToFP(expValue, typeMap[Context::TYPE_DOUBLE]);
  }

  builder.CreateRet(expValue);
}

void CodeGenVisitor::visitSReturnV(SReturnV *p) {
  builder.CreateRetVoid();
}

void CodeGenVisitor::visitSWhile(SWhile *p) {
  llvm::BasicBlock *condCheckBlock = MAKE_BASIC_BLOCK("while-cond");
  llvm::BasicBlock *bodyBlock = MAKE_BASIC_BLOCK("while-body");
  llvm::BasicBlock *nextBlock = MAKE_BASIC_BLOCK("while-merge");

  builder.CreateBr(condCheckBlock);

  builder.SetInsertPoint(condCheckBlock);
  p->exp_->accept(this);
  builder.CreateCondBr(expValue, bodyBlock, nextBlock);

  builder.SetInsertPoint(bodyBlock);
  p->stm_->accept(this);
  builder.CreateBr(condCheckBlock);

  builder.SetInsertPoint(nextBlock);
}

void CodeGenVisitor::visitSDoWhile(SDoWhile *p) {

  llvm::Value *conditionexpValue;
  llvm::Function *currentFunction =	builder.GetInsertBlock()->getParent();
  llvm::BasicBlock *whileBlock = llvm::BasicBlock::Create(llvmContext, "whileBlock", currentFunction);
  llvm::BasicBlock *afterwhileBlock = llvm::BasicBlock::Create(llvmContext, "afterwhileBlock", currentFunction);
  llvm::BasicBlock *condBlock = llvm::BasicBlock::Create(llvmContext, "condBlock", currentFunction);

  builder.CreateBr(whileBlock);

  builder.SetInsertPoint(whileBlock);
  
  p->stm_->accept(this);

  builder.CreateBr(condBlock);

  builder.SetInsertPoint(condBlock);

  p->exp_->accept(this);

  conditionexpValue = expValue;
  
  builder.CreateCondBr(conditionexpValue, whileBlock, afterwhileBlock);

  builder.SetInsertPoint(afterwhileBlock);

}

void CodeGenVisitor::visitSFor(SFor *p) {

	p->exp_1->accept(this);

  llvm::Value *startVal = expValue; 


	llvm::Function *currentFunction =	builder.GetInsertBlock()->getParent();

	llvm::BasicBlock *loopConditionBlock = llvm::BasicBlock::Create(llvmContext, "loopCondition", currentFunction);
	llvm::BasicBlock *loopBlock = llvm::BasicBlock::Create(llvmContext, "loopBlock", currentFunction);
	llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(llvmContext, "mergeBlock", currentFunction);
	
  builder.CreateBr(loopConditionBlock);
	
	builder.SetInsertPoint(loopConditionBlock);
  p->exp_2->accept(this);
	builder.CreateCondBr(expValue, loopBlock, mergeBlock);

	builder.SetInsertPoint(loopBlock);
	p->stm_->accept(this);
	p->exp_3->accept(this);
	builder.CreateBr(loopConditionBlock);

	builder.SetInsertPoint(mergeBlock);

}

void CodeGenVisitor::visitSBlock(SBlock *p) {
  beginCodeGenContext();

  for (auto stm : *p->liststm_) {
    stm->accept(this);
  }

  endCodeGenContext();
}

void CodeGenVisitor::visitSIfElse(SIfElse *p) {

  llvm::BasicBlock *ifBlock = MAKE_BASIC_BLOCK("ifBlock");
  llvm::BasicBlock *thenBlock = MAKE_BASIC_BLOCK("thenBlock");
  llvm::BasicBlock *elseBlock = MAKE_BASIC_BLOCK("elseBlock");
  llvm::BasicBlock *nextBlock = MAKE_BASIC_BLOCK("nextBlock");
  
  builder.CreateBr(ifBlock);
  builder.SetInsertPoint(ifBlock);
  p->exp_->accept(this);
  builder.CreateCondBr(expValue, thenBlock, elseBlock);

  builder.SetInsertPoint(thenBlock);
  p->stm_1->accept(this);
  builder.CreateBr(nextBlock);
  builder.SetInsertPoint(elseBlock);
  p->stm_2->accept(this);
  builder.CreateBr(nextBlock);
  builder.SetInsertPoint(nextBlock);
  
}

void CodeGenVisitor::visitEInt(EInt *p) {
  expValue = llvm::ConstantInt::get(llvmContext, TO_INT_VALUE(p->integer_));
}

void CodeGenVisitor::visitEDouble(EDouble *p) {
  expValue = llvm::ConstantFP::get(llvmContext, TO_DOUBLE_VALUE(p->double_));
}

void CodeGenVisitor::visitETrue(ETrue *p) {
  expValue = llvm::ConstantInt::get(llvmContext, TO_BOOL_VALUE(true));
}

void CodeGenVisitor::visitEFalse(EFalse *p) {
  expValue = llvm::ConstantInt::get(llvmContext, TO_BOOL_VALUE(false));
}

void CodeGenVisitor::visitEId(EId *p) {
  llvm::Value *ptr = codeGenContext->findSymbol(p->id_);
  if (ptr == nullptr) {
    throw "Oh no! Variable used before initializing it 😱";
  }

  expValue = builder.CreateLoad(ptr);
}

void CodeGenVisitor::visitEApp(EApp *p) {
  llvm::Function *func = module->getFunction(p->id_);

  const FunctionType *ft = currentContext->findFunction(func->getName());
  
  std::vector<llvm::Value *> arguments;
  int i = 0;
  for (auto arg : *p->listexp_) {
    arg->accept(this);

    if (ft->parameters[i] == Context::TYPE_DOUBLE && arg->type == Context::TYPE_INT) {
      expValue = builder.CreateSIToFP(expValue, typeMap[Context::TYPE_DOUBLE]);
    }

    arguments.push_back(expValue);
    i++;
  }

  expValue = builder.CreateCall(func, arguments);
}

void CodeGenVisitor::visitEProj(EProj *p) {
  p->exp_->accept(this);

  llvm::StructType *llvmSType = (llvm::StructType *) expValue->getType();

  expValue = genStructAccess(expValue, getMemberIndex(llvmSType, p->id_));
}

void CodeGenVisitor::visitEPIncr(EPIncr *p) {
  p->exp_->accept(this);

  llvm::Value *changedVal;
  if (p->type == Context::TYPE_INT) {
    changedVal = builder.CreateAdd(expValue, llvm::ConstantInt::get(llvmContext, TO_INT_VALUE(1)));
  }
  else if (p->type == Context::TYPE_DOUBLE) {
    changedVal = builder.CreateFAdd(expValue, llvm::ConstantFP::get(llvmContext, TO_DOUBLE_VALUE(1.0)));
  }
  else {
    UNREACHABLE_OH_NO
  }

  genAssignment(p->exp_, changedVal);
  // expValue remains the original value of p->exp_, from before the increment.
}

void CodeGenVisitor::visitEPDecr(EPDecr *p) {
  p->exp_->accept(this);

  llvm::Value *changedVal;
  if (p->type == Context::TYPE_INT) {
    changedVal = builder.CreateSub(expValue, llvm::ConstantInt::get(llvmContext, TO_INT_VALUE(1)));
  }
  else if (p->type == Context::TYPE_DOUBLE) {
    changedVal = builder.CreateFSub(expValue, llvm::ConstantFP::get(llvmContext, TO_DOUBLE_VALUE(1.0)));
  }
  else {
    UNREACHABLE_OH_NO
  }

  genAssignment(p->exp_, changedVal);
  // expValue remains the original value of p->exp_, from before the decrement.
}

void CodeGenVisitor::visitEIncr(EIncr *p) {
  p->exp_->accept(this);

  llvm::Value *changedVal;
  if (p->type == Context::TYPE_INT) {
    changedVal = builder.CreateAdd(expValue, llvm::ConstantInt::get(llvmContext, TO_INT_VALUE(1)));
  }
  else if (p->type == Context::TYPE_DOUBLE) {
    changedVal = builder.CreateFAdd(expValue, llvm::ConstantFP::get(llvmContext, TO_DOUBLE_VALUE(1.0)));
  }
  else {
    UNREACHABLE_OH_NO
  }

  expValue = genAssignment(p->exp_, changedVal);
}

void CodeGenVisitor::visitEDecr(EDecr *p) {
  p->exp_->accept(this);

  llvm::Value *changedVal;
  if (p->type == Context::TYPE_INT) {
    changedVal = builder.CreateSub(expValue, llvm::ConstantInt::get(llvmContext, TO_INT_VALUE(1)));
  }
  else if (p->type == Context::TYPE_DOUBLE) {
    changedVal = builder.CreateFSub(expValue, llvm::ConstantFP::get(llvmContext, TO_DOUBLE_VALUE(1.0)));
  }
  else {
    UNREACHABLE_OH_NO
  }

  expValue = genAssignment(p->exp_, changedVal);
}

void CodeGenVisitor::visitEUPlus(EUPlus *p) {
  // Unary plus does not change the value of the expression.
  p->exp_->accept(this);
}

void CodeGenVisitor::visitEUMinus(EUMinus *p) {
  p->exp_->accept(this);

  if (p->type == Context::TYPE_INT) {
    expValue = builder.CreateNeg(expValue);
  }
  else if (p->type == Context::TYPE_DOUBLE) {
    expValue = builder.CreateFNeg(expValue);
  }
  else {
    UNREACHABLE_OH_NO
  }
}

void CodeGenVisitor::visitETimes(ETimes *p) {
  p->exp_1->accept(this);
  llvm::Value *firstValue = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondValue = expValue;

  const BasicType *firstType = p->exp_1->type;
  const BasicType *secondType = p->exp_2->type;
  const BasicType *resultType;

  handleNumberConversions(firstType, &firstValue, secondType, &secondValue, &resultType);

  if(resultType == Context::TYPE_INT){
    expValue = builder.CreateMul(firstValue, secondValue);
  }
  else if(resultType == Context::TYPE_DOUBLE){
    expValue = builder.CreateFMul(firstValue, secondValue);
  }
  else{
    UNREACHABLE_OH_NO
  }
}

void CodeGenVisitor::visitEDiv(EDiv *p) {

  p->exp_1->accept(this);
  llvm::Value *firstexpValue = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondexpValue = expValue;

  const BasicType *firstexpType = p->exp_1->type;
  const BasicType *secondexpType = p->exp_2->type;
  const BasicType *resultType;
  handleNumberConversions(firstexpType, &firstexpValue, secondexpType, &secondexpValue, &resultType);
  
  if(resultType == Context::TYPE_INT){
    expValue = builder.CreateSDiv(firstexpValue, secondexpValue);
  }
  else {
    if(resultType == Context::TYPE_DOUBLE){
    expValue = builder.CreateFDiv(firstexpValue,secondexpValue);
    }
    else{
      UNREACHABLE_OH_NO
    }
  }
}

void CodeGenVisitor::visitEPlus(EPlus *p) {
  p->exp_1->accept(this);
  llvm::Value *firstexpValue = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondexpValue = expValue;

  const BasicType *firstexpType = p->exp_1->type;
  const BasicType *secondexpType = p->exp_2->type;
  const BasicType *resultType;
  handleNumberConversions(firstexpType, &firstexpValue, secondexpType, &secondexpValue, &resultType);
  
  if(resultType == Context::TYPE_INT){
    expValue = builder.CreateAdd(firstexpValue, secondexpValue);
  }else{
    if(resultType == Context::TYPE_DOUBLE){
    expValue = builder.CreateFAdd(firstexpValue,secondexpValue);
    }else{
      UNREACHABLE_OH_NO
    }
  }
}

void CodeGenVisitor::visitEMinus(EMinus *p) {
  p->exp_1->accept(this);
  llvm::Value *firstVal = expValue;
  const BasicType *firstType = p->exp_1->type;

  p->exp_2->accept(this);
  llvm::Value *secondVal = expValue;
  const BasicType *secondType = p->exp_2->type;

  const BasicType *resultType;

  handleNumberConversions(firstType, &firstVal, secondType, &secondVal, &resultType);

  if (resultType == Context::TYPE_INT) {
    expValue = builder.CreateSub(firstVal, secondVal);
  }
  else if (resultType == Context::TYPE_DOUBLE) {
    expValue = builder.CreateFSub(firstVal, secondVal);
  }
  else {
    UNREACHABLE_OH_NO
  }
}

void CodeGenVisitor::visitETwc(ETwc *p) {
  p->exp_1->accept(this);
  llvm::Value *firstVal = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondVal = expValue;

  const BasicType *firstType = p->exp_1->type;
  const BasicType *secondType = p->exp_2->type;
  const BasicType *resultType;

  handleNumberConversions(firstType, &firstVal, secondType, &secondVal, &resultType);
  
  llvm::Value *lt, *gt;

  if (resultType == Context::TYPE_INT) {
    lt = builder.CreateICmpSLT(firstVal, secondVal);
    gt = builder.CreateICmpSGT(firstVal, secondVal);
  }
  else {
    lt = builder.CreateFCmpULT(firstVal, secondVal);
    gt = builder.CreateFCmpUGT(firstVal, secondVal);
  }

  // Extend lt and gt to i64, they are i1 and we need a (possibly negative)
  // i64 return.
  lt = builder.CreateSExt(lt, typeMap[Context::TYPE_INT]);
  gt = builder.CreateSExt(gt, typeMap[Context::TYPE_INT]);

  expValue = builder.CreateSub(gt, lt);
}

void CodeGenVisitor::visitELt(ELt *p) {
  p->exp_1->accept(this);
  llvm::Value *firstexpValue = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondexpValue = expValue;

  const BasicType *firstexpType = p->exp_1->type;
  const BasicType *secondexpType = p->exp_2->type;
  const BasicType *resultType;
  handleNumberConversions(firstexpType, &firstexpValue, secondexpType, &secondexpValue, &resultType);
  
  if(resultType == Context::TYPE_INT){
    expValue = builder.CreateICmpSLT(firstexpValue, secondexpValue);
  }else{
    if(resultType == Context::TYPE_DOUBLE){
      expValue = builder.CreateFCmpULT(firstexpValue,secondexpValue);
    }else{
      UNREACHABLE_OH_NO
    }
  }
}

void CodeGenVisitor::visitEGt(EGt *p) {
  p->exp_1->accept(this);
  llvm::Value *firstVal = expValue;
  const BasicType *firstType = p->exp_1->type;

  p->exp_2->accept(this);
  llvm::Value *secondVal = expValue;
  const BasicType *secondType = p->exp_2->type;

  const BasicType *resultType;

  handleNumberConversions(firstType, &firstVal, secondType, &secondVal, &resultType);

  if (resultType == Context::TYPE_INT) {
    expValue = builder.CreateICmpSGT(firstVal, secondVal);
  }
  else if (resultType == Context::TYPE_DOUBLE) {
    expValue = builder.CreateFCmpUGT(firstVal, secondVal);
  }
  else {
    UNREACHABLE_OH_NO
  }
}

void CodeGenVisitor::visitELtEq(ELtEq *p) {
  p->exp_1->accept(this);
  llvm::Value *firstValue = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondValue = expValue;

  const BasicType *firstType = p->exp_1->type;
  const BasicType *secondType = p->exp_2->type;
  const BasicType *resultType;

  handleNumberConversions(firstType, &firstValue, secondType, &secondValue, &resultType);

  if(resultType == Context::TYPE_INT){
    expValue = builder.CreateICmpSLE(firstValue, secondValue);
  }
  else if(resultType == Context::TYPE_DOUBLE){
    expValue = builder.CreateFCmpULE(firstValue, secondValue);
  }
  else{
    UNREACHABLE_OH_NO
  }
}
  


void CodeGenVisitor::visitEGtEq(EGtEq *p) {

  p->exp_1->accept(this);
  llvm::Value *firstValue = expValue;
  p->exp_2->accept(this);
  llvm::Value *secondValue = expValue;

  const BasicType *firstType = p->exp_1->type;
  const BasicType *secondType = p->exp_2->type;
  const BasicType *resultType;

  handleNumberConversions(firstType, &firstValue, secondType, &secondValue, &resultType);

  if(resultType == Context::TYPE_INT){
    expValue = builder.CreateICmpSGE(firstValue, secondValue);
  }
  else if(resultType == Context::TYPE_DOUBLE){
    expValue = builder.CreateFCmpUGE(firstValue, secondValue);
  }
  else{
    UNREACHABLE_OH_NO
  }
}

void CodeGenVisitor::visitEEq(EEq *p) {
  p->exp_1->accept(this);
  llvm::Value *firstVal = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondVal = expValue;

  expValue = genEqComparison(p->exp_1->type, firstVal, p->exp_2->type, secondVal);
}

void CodeGenVisitor::visitENEq(ENEq *p) {
  p->exp_1->accept(this);
  llvm::Value *firstVal = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondVal = expValue;

  llvm::Value *eqValue = genEqComparison(p->exp_1->type, firstVal, p->exp_2->type, secondVal);
  expValue = builder.CreateXor(eqValue, 1);
}

void CodeGenVisitor::visitEAnd(EAnd *p) {
  p->exp_1->accept(this);
  llvm::Value *firstVal = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondVal = expValue;

  expValue = builder.CreateAnd(firstVal, secondVal);
}

void CodeGenVisitor::visitEOr(EOr *p) {
  p->exp_1->accept(this);
  llvm::Value *firstVal = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondVal = expValue;

  expValue = builder.CreateOr(firstVal, secondVal);
}

void CodeGenVisitor::visitEAss(EAss *p) {
  p->exp_2->accept(this);
  expValue = genAssignment(p->exp_1, expValue);
}

void CodeGenVisitor::visitECond(ECond *p) {
  p->exp_1->accept(this);

  llvm::BasicBlock *trueBlock = MAKE_BASIC_BLOCK("cond-true");
  llvm::BasicBlock *falseBlock = MAKE_BASIC_BLOCK("cond-false");
  llvm::BasicBlock *nextBlock = MAKE_BASIC_BLOCK("cond-merge");

  llvm::Value *condValue = expValue;
  builder.CreateCondBr(condValue, trueBlock, falseBlock);

  builder.SetInsertPoint(trueBlock);
  p->exp_2->accept(this);
  llvm::Value *trueBlockResult = expValue;
  builder.CreateBr(nextBlock);

  builder.SetInsertPoint(falseBlock);
  p->exp_3->accept(this);
  llvm::Value *falseBlockResult = expValue;
  builder.CreateBr(nextBlock);

  builder.SetInsertPoint(nextBlock);
  expValue = builder.CreateSelect(condValue, trueBlockResult, falseBlockResult);
}

llvm::Value *CodeGenVisitor::genEqComparison(const BasicType *firstType, llvm::Value *firstVal,
  const BasicType *secondType, llvm::Value *secondVal
) {
  const BasicType *commonType;
  if (handleNumberConversions(firstType, &firstVal, secondType, &secondVal, &commonType)) {
    if (commonType == Context::TYPE_INT) {
      return builder.CreateICmpEQ(firstVal, secondVal);
    }
    else if (commonType == Context::TYPE_DOUBLE) {
      return builder.CreateFCmpUEQ(firstVal, secondVal);
    }
    else {
      UNREACHABLE_OH_NO
    }
  }
  else if (firstType == secondType) {
    if (firstType == Context::TYPE_BOOL){
      return builder.CreateICmpEQ(firstVal, secondVal);
    }
    else if (firstType == Context::TYPE_VOID) {
      UNREACHABLE_OH_NO
    }
    else {
        llvm::StructType *llvmSType = (llvm::StructType *) firstVal->getType();
        unsigned int numElements = llvmSType->getNumElements();

        const StructType *sType = (const StructType *) currentContext->findBasicType(llvmSType->getName());
        auto structMembers = sType->getMemberTypes();

        llvm::Value *overallResult = llvm::ConstantInt::get(llvmContext, TO_BOOL_VALUE(true));

        for (unsigned int i = 0; i < numElements; i++) {
          const BasicType *memberType = structMembers[i];

          llvm::Value *memberInFirst = genStructAccess(firstVal, i);
          llvm::Value *memberInSecond = genStructAccess(secondVal, i);

          llvm::Value *compResult = genEqComparison(memberType, memberInFirst, memberType, memberInSecond);

          overallResult = builder.CreateAnd(overallResult, compResult);
        }

        return overallResult;
      }
  }
  else {
    // always false
    return llvm::ConstantInt::get(llvmContext, TO_BOOL_VALUE(false));
  }
}

llvm::Value *CodeGenVisitor::genStructAccess(llvm::Value *structVal, int memberIndex) {
  return builder.CreateExtractValue(structVal, memberIndex);
}

llvm::Value *CodeGenVisitor::genAssignment(Exp *e1, llvm::Value *assignedValue) {
  EId *eId = dynamic_cast<EId *>(e1);
  EProj *eProj = dynamic_cast<EProj *>(e1);

  if (e1->type == Context::TYPE_DOUBLE && assignedValue->getType() == typeMap[Context::TYPE_INT]) {
    assignedValue = builder.CreateSIToFP(assignedValue, typeMap[Context::TYPE_DOUBLE]);
  }

  if (eId != nullptr) {
    builder.CreateStore(assignedValue, codeGenContext->findSymbol(eId->id_));
    return assignedValue;
  }
  else if (eProj != nullptr) {
    // bla.foo = x;
    // => insertValue bla, x, idxof(foo)

    // (bla.foo).bar = x;
    // => insertValue bla, x, idxof(foo), idxof(bar)

    std::vector<std::string> ids;

    EProj *currentProj = eProj;
    while (true) {
      ids.push_back(currentProj->id_);

      EId *eCurrentId = dynamic_cast<EId *>(currentProj->exp_);
      if (eCurrentId != nullptr) {
        ids.push_back(eCurrentId->id_);
        break;
      }

      currentProj = dynamic_cast<EProj *>(currentProj->exp_);
    }

    llvm::Value *rootValue = codeGenContext->findSymbol(ids[ids.size() - 1]);

    llvm::StructType *currentLLVMSType = (llvm::StructType *) rootValue->getType();

    std::vector<unsigned int> indices;
    for (int i = ids.size() - 2; i >= 0; i--) {
      int memberIndex = getMemberIndex(currentLLVMSType, ids[i]);
      
      indices.push_back(memberIndex);
      currentLLVMSType = llvm::dyn_cast<llvm::StructType>(currentLLVMSType->getElementType(memberIndex));
    }

    return builder.CreateInsertValue(rootValue, assignedValue, indices);
  }
  else {
    UNREACHABLE_OH_NO
  }
}

int CodeGenVisitor::getMemberIndex(llvm::StructType *llvmSType, std::string& memberName) {
  const StructType *sType = (const StructType *) currentContext->findBasicType(llvmSType->getName());

  std::vector<std::string> names = sType->getMemberNames();
  for (int i = 0; i < names.size(); i++) {
    if (names[i] == memberName) {
      return i;
    }
  }

  UNREACHABLE_OH_NO
}

// Takes two arbitrary values and their types.
// If both types are number types, converts them both to the same type, writes that type to resultType and returns true.
// If either type is not a number, does not perform any conversion, writes nullptr to resultType and returns false.
bool CodeGenVisitor::handleNumberConversions(const BasicType *firstType, llvm::Value **firstVal,
  const BasicType *secondType, llvm::Value **secondVal,
  const BasicType **resultType
) {
  if (firstType == secondType) {
    if (firstType == Context::TYPE_INT) {
      *resultType = Context::TYPE_INT;
      return true;
    }
    if (firstType == Context::TYPE_DOUBLE) {
      *resultType = Context::TYPE_DOUBLE;
      return true;
    }

    *resultType = nullptr;
    return false;
  }

  if (firstType == Context::TYPE_INT && secondType == Context::TYPE_DOUBLE) {
    *firstVal = builder.CreateSIToFP(*firstVal, typeMap[Context::TYPE_DOUBLE]);
    *resultType = Context::TYPE_DOUBLE;
    return true;
  }
  if (firstType == Context::TYPE_DOUBLE && secondType == Context::TYPE_INT) {
    *secondVal = builder.CreateSIToFP(*secondVal, typeMap[Context::TYPE_DOUBLE]);
    *resultType = Context::TYPE_DOUBLE;
    return true;
  }

  *resultType = nullptr;
  return false;
}