//
//  LLVMExternalGlobalDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/GlobalVariable.h>

#include "wisey/LLVMExternalGlobalDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMExternalGlobalDeclaration::LLVMExternalGlobalDeclaration(const ITypeSpecifier* typeSpecifier,
                                                             string name) :
mTypeSpecifier(typeSpecifier),
mName(name) {
}

LLVMExternalGlobalDeclaration::~LLVMExternalGlobalDeclaration() {
  delete mTypeSpecifier;
}

IObjectType* LLVMExternalGlobalDeclaration::prototypeObject(IRGenerationContext& context,
                                                    ImportProfile* importProfile) const {
  return NULL;
}

void LLVMExternalGlobalDeclaration::prototypeMethods(IRGenerationContext& context) const {
  const IType* variableType = mTypeSpecifier->getType(context);
  Type* variableLLVMType = variableType->getLLVMType(context);
  GlobalVariable* global = new GlobalVariable(*context.getModule(),
                                              variableLLVMType,
                                              false,
                                              GlobalValue::ExternalLinkage,
                                              nullptr,
                                              mName.c_str());
  global->setAlignment(8);
  context.setLLVMGlobalVariable(variableType, mName);
}

void LLVMExternalGlobalDeclaration::generateIR(IRGenerationContext& context) const {
}
