#include "CodeGenVisitor.h"

CodeGenVisitor::CodeGenVisitor(Context *context) : llvmContext(), builder(llvmContext) {
  currentContext = context;

  module = llvm::make_unique<llvm::Module>("CPP", llvmContext);

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

  // Insert arguments into new child context
  for (auto& arg : llvmF->args()) {
    codeGenContext->addSymbol(arg.getName(), &arg);
  }

  // Codegen statements for function body

  llvm::BasicBlock *bb = llvm::BasicBlock::Create(llvmContext, "entry", llvmF);
  builder.SetInsertPoint(bb);

  for (auto stm : *p->liststm_) {
    stm->accept(this);
  }

  // TODO: Returns for void functions

  endCodeGenContext();

  llvm::verifyFunction(*llvmF, &llvm::outs());
}

void CodeGenVisitor::visitSReturn(SReturn *p) {
  p->exp_->accept(this);

  builder.CreateRet(expValue);
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
  expValue = codeGenContext->findSymbol(p->id_);
}

void CodeGenVisitor::visitEEq(EEq *p) {
  p->exp_1->accept(this);
  llvm::Value *firstVal = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondVal = expValue;

  expValue = genEqComparison(p->exp_1->type, firstVal, p->exp_2->type, secondVal);
}

llvm::Value *CodeGenVisitor::genEqComparison(const BasicType *firstType, llvm::Value *firstVal,
  const BasicType *secondType, llvm::Value *secondVal
) {
  if (firstType == secondType) {
    if (firstType == Context::TYPE_INT) {
      return builder.CreateICmpEQ(firstVal, secondVal);
    }
    else if (firstType == Context::TYPE_DOUBLE) {
      return builder.CreateFCmpUEQ(firstVal, secondVal);
    }
    else if (firstType == Context::TYPE_BOOL){
      return builder.CreateICmpEQ(firstVal, secondVal);
    }
    else if (firstType == Context::TYPE_VOID) {
      /* unreachable, type error */ return nullptr;
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
  } else {
    if (firstType == Context::TYPE_INT && secondType == Context::TYPE_DOUBLE) {
      // convert value 1 to double and compare
      return builder.CreateFCmpUEQ(
        builder.CreateSIToFP(firstVal, typeMap[Context::TYPE_DOUBLE]),
        secondVal);
    }
    else if(firstType == Context::TYPE_DOUBLE && secondType == Context::TYPE_INT) {
      // convert value 2 to double and compare
      return builder.CreateFCmpUEQ(
        firstVal,
        builder.CreateSIToFP(secondVal, typeMap[Context::TYPE_DOUBLE]));
    }
    else {
      // always false
      return llvm::ConstantInt::get(llvmContext, TO_BOOL_VALUE(false));
    }
  }
}

llvm::Value *CodeGenVisitor::genStructAccess(llvm::Value *structVal, int memberIndex) {
  return builder.CreateExtractValue(structVal, memberIndex);
}