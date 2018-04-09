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
                                                 AccessLevel accessLevel,
                                                 const ITypeSpecifier* returnSpecifier,
                                                 std::vector<const ITypeSpecifier*>
                                                 argumentSpecifiers) :
mName(name),
mAccessLevel(accessLevel),
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
  for (const ITypeSpecifier* argumentSpecifier : mArgumentSpecifiers) {
    argumentTypes.push_back(argumentSpecifier->getType(context));
  }
  const IType* returnType = mReturnSpecifier->getType(context);
  LLVMFunctionType* functionType = context.getLLVMFunctionType(returnType, argumentTypes);
  context.registerLLVMFunctionNamedType(mName, mAccessLevel, functionType);
  
  Function::Create(functionType->getLLVMType(context),
                   GlobalValue::ExternalLinkage,
                   mName,
                   context.getModule());
}

void LLVMFunctionDeclaration::generateIR(IRGenerationContext& context) const {
}
