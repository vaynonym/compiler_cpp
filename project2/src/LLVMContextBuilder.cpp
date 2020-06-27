#include "LLVMContextBuilder.h"

LLVMContextBuilder::LLVMContextBuilder(Context *context, llvm::LLVMContext *llvmContext, llvm::IRBuilder<> *builder,
        std::map<const BasicType *, llvm::Type *> *typeMap, llvm::Module *module)
    : context(context), llvmContext(llvmContext), builder(builder), typeMap(typeMap), module(module) {

  createTypeObjects();
}

void LLVMContextBuilder::createTypeObjects() {
  // Go through all types in context:
  //   Create llvm::TYpe object for it
  //   Insert into Type->llvm:Type map

  const std::vector<const BasicType *> knownTypes = context->getKnownTypes();
  for (const BasicType *type : knownTypes) {
    llvm::Type *llvmType;
    if (type == Context::TYPE_INT) {
      llvmType = llvm::Type::getInt64Ty(*llvmContext);
    }
    else if (type == Context::TYPE_DOUBLE) {
      llvmType = llvm::Type::getDoubleTy(*llvmContext);
    }
    else if (type == Context::TYPE_BOOL) {
      llvmType = llvm::Type::getInt1Ty(*llvmContext);
    }
    else if (type == Context::TYPE_VOID) {
      llvmType = llvm::Type::getVoidTy(*llvmContext);
    }
    else {
      const StructType *sType = (const StructType *) type;
      auto members = sType->getMemberTypes();

      std::vector<llvm::Type *> memberTypes;
      for (auto type : members) {
        memberTypes.push_back((*typeMap)[type]);
      }

      llvm::StructType *llvmSType = llvm::StructType::create(*llvmContext, memberTypes, sType->id);

      llvmType = llvmSType;
    }

    typeMap->emplace(type, llvmType);
  }
}

void LLVMContextBuilder::visitPDefs(PDefs *p) {
  for (auto def : *p->listdef_) {
    def->accept(this);
  }
}

void LLVMContextBuilder::visitDFun(DFun *p) {
  const FunctionType *ft = context->findFunction(p->id_);

  // Map the typechecker's BasicType objects to corresponding llvm::Type objects
  std::vector<llvm::Type *> arguments;
  for (const BasicType *argType : ft->parameters) {
    arguments.push_back((*typeMap)[argType]);
  }

  llvm::Type *returnType = (*typeMap)[ft->returnType];

  // Create LLVM FunctionType
  llvm::FunctionType *llvmFt = llvm::FunctionType::get(returnType, arguments, false);

  // Create LLVM Function
  llvm::Function *llvmF = llvm::Function::Create(llvmFt, llvm::Function::ExternalLinkage, p->id_, module);

  // Set argument names
  int i = 0;
  for (auto& arg : llvmF->args()) {
    arg.setName(((ADecl *) (*p->listarg_)[i++])->id_);
  }
}