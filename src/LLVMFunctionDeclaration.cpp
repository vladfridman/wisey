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
                                                 bool isExternal,
                                                 const ITypeSpecifier* returnSpecifier,
                                                 std::vector<const ITypeSpecifier*>
                                                 argumentSpecifiers) :
mName(name),
mIsExternal(isExternal),
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

LLVMFunctionDeclaration* LLVMFunctionDeclaration::
createInternal(std::string name,
               const ITypeSpecifier* returnSpecifier,
               std::vector<const ITypeSpecifier*> argumentSpecifiers) {
  return new LLVMFunctionDeclaration(name, false, returnSpecifier, argumentSpecifiers);
}

LLVMFunctionDeclaration* LLVMFunctionDeclaration::
createExternal(std::string name,
               const ITypeSpecifier* returnSpecifier,
               std::vector<const ITypeSpecifier*> argumentSpecifiers) {
  return new LLVMFunctionDeclaration(name, true, returnSpecifier, argumentSpecifiers);
}

IObjectType* LLVMFunctionDeclaration::prototypeObject(IRGenerationContext& context,
                                                      ImportProfile* importProfile) const {
  return NULL;
}

void LLVMFunctionDeclaration::prototypeMethods(IRGenerationContext& context) const {
  vector<const IType*> argumentTypes;
  for (const ITypeSpecifier* argumentSpecifier : mArgumentSpecifiers) {
    argumentTypes.push_back(argumentSpecifier->getType(context));
  }
  const IType* returnType = mReturnSpecifier->getType(context);
  LLVMFunctionType* functionType = context.getLLVMFunctionType(returnType, argumentTypes);
  if (mIsExternal) {
    context.registerLLVMExternalFunctionNamedType(mName, functionType);
  } else {
    context.registerLLVMInternalFunctionNamedType(mName, functionType);
  }
  
  Function::Create(functionType->getLLVMType(context),
                   GlobalValue::ExternalLinkage,
                   mName,
                   context.getModule());
}

void LLVMFunctionDeclaration::generateIR(IRGenerationContext& context) const {
}
