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
      // TODO: Structs oh no 😱
      continue;
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

  llvm::BasicBlock *bb = llvm::BasicBlock::Create(llvmContext, "entry", llvmF);
  builder.SetInsertPoint(bb);

  // Codegen statements for function body

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
  expValue = llvm::ConstantInt::get(llvmContext, llvm::APInt(INTEGER_WIDTH, p->integer_, true));
}

void CodeGenVisitor::visitEDouble(EDouble *p) {
  expValue = llvm::ConstantFP::get(llvmContext, llvm::APFloat(p->double_));
}

void CodeGenVisitor::visitETrue(ETrue *p) {
  expValue = llvm::ConstantInt::get(llvmContext, llvm::APInt(BOOLEAN_WIDTH, 1, false));
}

void CodeGenVisitor::visitEFalse(EFalse *p) {
  expValue = llvm::ConstantInt::get(llvmContext, llvm::APInt(BOOLEAN_WIDTH, 0, false));
}

void CodeGenVisitor::visitEId(EId *p) {
  expValue = codeGenContext->findSymbol(p->id_);
}

void CodeGenVisitor::visitEEq(EEq *p) {
  p->exp_1->accept(this);
  llvm::Value *firstVal = expValue;

  p->exp_2->accept(this);
  llvm::Value *secondVal = expValue;

  if (p->exp_1->type == p->exp_2->type) {
    const BasicType *t = p->exp_1->type;

    if (t == Context::TYPE_INT) {
      expValue = builder.CreateICmpEQ(firstVal, secondVal);
    }
    else if (t == Context::TYPE_DOUBLE) {
      expValue = builder.CreateFCmpUEQ(firstVal, secondVal);
    }
    else if (t == Context::TYPE_BOOL) {
      expValue = builder.CreateICmpEQ(firstVal, secondVal);
    }
    else if (t == Context::TYPE_VOID) {
      // type error, unreachable
    }
    else {
      // TODO: Structs oh no 😱
      return;
    }
  } else {
    const BasicType *t1 = p->exp_1->type;
    const BasicType *t2 = p->exp_1->type;

    if (t1 == Context::TYPE_INT && t2 == Context::TYPE_DOUBLE) {
      // convert value 1 to double and compare
      expValue = builder.CreateFCmpUEQ(
        builder.CreateSIToFP(firstVal, typeMap[Context::TYPE_DOUBLE]),
        secondVal);
    }
    else if(t1 == Context::TYPE_DOUBLE && t2 == Context::TYPE_INT) {
      // convert value 2 to double and compare
      expValue = builder.CreateFCmpUEQ(
        firstVal,
        builder.CreateSIToFP(secondVal, typeMap[Context::TYPE_DOUBLE]));
    }
    else {
      // always false
      expValue = llvm::ConstantInt::get(llvmContext, llvm::APInt(BOOLEAN_WIDTH, 0, false));
    }
  }
}