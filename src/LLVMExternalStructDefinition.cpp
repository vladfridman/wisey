//
//  LLVMExternalStructDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMExternalStructDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMExternalStructDefinition::LLVMExternalStructDefinition(string name,
                                                           vector<const ILLVMTypeSpecifier*>
                                                           typeSpecifiers) :
mName(name),
mTypeSpecifiers(typeSpecifiers) {
}

LLVMExternalStructDefinition::~LLVMExternalStructDefinition() {
  for (const ILLVMTypeSpecifier* typeSpecifier : mTypeSpecifiers) {
    delete typeSpecifier;
  }
  mTypeSpecifiers.clear();
}

IObjectType* LLVMExternalStructDefinition::prototypeObject(IRGenerationContext& context) const {
  StructType* structType = context.getModule()->getTypeByName(mName);
  assert(structType == NULL);
  
  structType = StructType::create(context.getLLVMContext(), mName);
  LLVMStructType* llvmStructType = LLVMStructType::newExternalLLVMStructType(structType);
  context.addLLVMStructType(llvmStructType);
  
  return NULL;
}

void LLVMExternalStructDefinition::prototypeMethods(IRGenerationContext& context) const {
  LLVMStructType* llvmStructType = context.getLLVMStructType(mName);
  
  vector<const ILLVMType*> structTypes;
  for (const ILLVMTypeSpecifier* llvmTypeSpecifier : mTypeSpecifiers) {
    const ILLVMType* type = llvmTypeSpecifier->getType(context);
    structTypes.push_back(type);
  }
  llvmStructType->setBodyTypes(context, structTypes);
}

Value* LLVMExternalStructDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

