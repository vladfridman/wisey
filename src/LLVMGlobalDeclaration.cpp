//
//  LLVMGlobalDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/GlobalVariable.h>

#include "wisey/LLVMGlobalDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMGlobalDeclaration::LLVMGlobalDeclaration(const ITypeSpecifier* typeSpecifier,
                                             string name,
                                             int line) :
mTypeSpecifier(typeSpecifier), mName(name), mLine(line) {
}

LLVMGlobalDeclaration::~LLVMGlobalDeclaration() {
  delete mTypeSpecifier;
}

IObjectType* LLVMGlobalDeclaration::prototypeObject(IRGenerationContext& context,
                                                    ImportProfile* importProfile) const {
  return NULL;
}

void LLVMGlobalDeclaration::prototypeMethods(IRGenerationContext& context) const {
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

void LLVMGlobalDeclaration::generateIR(IRGenerationContext& context) const {
}

int LLVMGlobalDeclaration::getLine() const {
  return mLine;
}
