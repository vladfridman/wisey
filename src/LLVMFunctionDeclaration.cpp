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
                                                 bool isVarArg,
                                                 const ITypeSpecifier* returnSpecifier,
                                                 std::vector<const ITypeSpecifier*>
                                                 argumentSpecifiers,
                                                 int line) :
mName(name),
mIsExternal(isExternal),
mIsVarArg(isVarArg),
mReturnSpecifier(returnSpecifier),
mArgumentSpecifiers(argumentSpecifiers),
mLine(line) {
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
               std::vector<const ITypeSpecifier*> argumentSpecifiers,
               int line) {
  return new LLVMFunctionDeclaration(name, false, false, returnSpecifier, argumentSpecifiers, line);
}

LLVMFunctionDeclaration* LLVMFunctionDeclaration::
createInternalWithVarArg(std::string name,
                         const ITypeSpecifier* returnSpecifier,
                         std::vector<const ITypeSpecifier*> argumentSpecifiers,
                         int line) {
  return new LLVMFunctionDeclaration(name, false, true, returnSpecifier, argumentSpecifiers, line);
}

LLVMFunctionDeclaration* LLVMFunctionDeclaration::
createExternal(std::string name,
               const ITypeSpecifier* returnSpecifier,
               std::vector<const ITypeSpecifier*> argumentSpecifiers,
               int line) {
  return new LLVMFunctionDeclaration(name, true, false, returnSpecifier, argumentSpecifiers, line);
}

LLVMFunctionDeclaration* LLVMFunctionDeclaration::
createExternalWithVarArg(std::string name,
                         const ITypeSpecifier* returnSpecifier,
                         std::vector<const ITypeSpecifier*> argumentSpecifiers,
                         int line) {
  return new LLVMFunctionDeclaration(name, true, true, returnSpecifier, argumentSpecifiers, line);
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
  LLVMFunctionType* functionType = mIsVarArg
  ? context.getLLVMFunctionTypeWithVarArg(returnType, argumentTypes)
  : context.getLLVMFunctionType(returnType, argumentTypes);
  if (mIsExternal) {
    context.registerLLVMExternalFunctionNamedType(mName, functionType, mLine);
  } else {
    context.registerLLVMInternalFunctionNamedType(mName, functionType, mLine);
  }
  
  Function::Create(functionType->getLLVMType(context),
                   GlobalValue::ExternalLinkage,
                   mName,
                   context.getModule());
}

void LLVMFunctionDeclaration::generateIR(IRGenerationContext& context) const {
}
