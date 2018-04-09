//
//  LLVMFunctionDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMFunctionDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMFunctionDeclaration::LLVMFunctionDeclaration(std::string name,
                                                 const ITypeSpecifier* returnSpecifier,
                                                 std::vector<const ITypeSpecifier*>
                                                 argumentSpecifiers) :
mName(name),
mReturnSpecifier(returnSpecifier),
mArgumentSpecifiers(argumentSpecifiers) {
}

LLVMFunctionDeclaration::~LLVMFunctionDeclaration() {
  delete mReturnSpecifier;
  for (const ITypeSpecifier* typeSpecifier : mArgumentSpecifiers) {
    delete typeSpecifier;
  }
  mArgumentSpecifiers.clear();
}

IObjectType* LLVMFunctionDeclaration::prototypeObject(IRGenerationContext& context) const {
  return NULL;
}

void LLVMFunctionDeclaration::prototypeMethods(IRGenerationContext& context) const {
  vector<const IType*> argumentTypes;
  vector<Type*> llvmArgumentTypes;
  for (const ITypeSpecifier* argumentSpecifier : mArgumentSpecifiers) {
    const IType* argumentType = argumentSpecifier->getType(context);
    argumentTypes.push_back(argumentType);
    llvmArgumentTypes.push_back(argumentType->getLLVMType(context));
  }
  const IType* returnType = mReturnSpecifier->getType(context);
  LLVMFunctionType* functionType = context.getLLVMFunctionType(returnType, argumentTypes);
  context.registerLLVMFunctionNamedType(mName, PUBLIC_ACCESS, functionType);
  Type* llvmReturnType = returnType->getLLVMType(context);
  FunctionType* llvmFunctionType = FunctionType::get(llvmReturnType, llvmArgumentTypes, false);
  
  Function::Create(llvmFunctionType, GlobalValue::ExternalLinkage, mName, context.getModule());
}

void LLVMFunctionDeclaration::generateIR(IRGenerationContext& context) const {
}

