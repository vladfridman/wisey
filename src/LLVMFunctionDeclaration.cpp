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
  vector<Type*> argumentTypes;
  for (const ITypeSpecifier* argumentSpecifier : mArgumentSpecifiers) {
    argumentTypes.push_back(argumentSpecifier->getType(context)->getLLVMType(context));
  }
  Type* llvmReturnType = mReturnSpecifier->getType(context)->getLLVMType(context);
  FunctionType* ftype = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  Function::Create(ftype, GlobalValue::ExternalLinkage, mName, context.getModule());
}

Value* LLVMFunctionDeclaration::generateIR(IRGenerationContext& context) const {
  return NULL;
}

