//
//  LLVMExternalStructDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "LLVMExternalStructDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMExternalStructDefinition::LLVMExternalStructDefinition(string name,
                                                           vector<const ITypeSpecifier*>
                                                           typeSpecifiers,
                                                           int line) :
mName(name),
mTypeSpecifiers(typeSpecifiers),
mLine(line) {
}

LLVMExternalStructDefinition::~LLVMExternalStructDefinition() {
  for (const ITypeSpecifier* typeSpecifier : mTypeSpecifiers) {
    delete typeSpecifier;
  }
  mTypeSpecifiers.clear();
}

IObjectType* LLVMExternalStructDefinition::prototypeObject(IRGenerationContext& context,
                                                           ImportProfile* importProfile) const {
  StructType* structType = context.getModule()->getTypeByName(mName);
  assert(structType == NULL);
  
  structType = StructType::create(context.getLLVMContext(), mName);
  LLVMStructType* llvmStructType = LLVMStructType::newExternalLLVMStructType(structType);
  context.addLLVMStructType(llvmStructType, mLine);
  
  return NULL;
}

void LLVMExternalStructDefinition::prototypeMethods(IRGenerationContext& context) const {
  LLVMStructType* llvmStructType = context.getLLVMStructType(mName, mLine);
  
  vector<const IType*> structTypes;
  for (const ITypeSpecifier* llvmTypeSpecifier : mTypeSpecifiers) {
    const IType* type = llvmTypeSpecifier->getType(context);
    structTypes.push_back(type);
  }
  if (structTypes.size()) {
    llvmStructType->setBodyTypes(context, structTypes);
  }
}

void LLVMExternalStructDefinition::generateIR(IRGenerationContext& context) const {
}
