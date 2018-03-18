//
//  LLVMStructDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/LLVMStructDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

LLVMStructDefinition::LLVMStructDefinition(string name,
                                           vector<const ILLVMTypeSpecifier*> typeSpecifiers) :
mName(name),
mTypeSpecifiers(typeSpecifiers) {
}

LLVMStructDefinition::~LLVMStructDefinition() {
  for (const ILLVMTypeSpecifier* typeSpecifier : mTypeSpecifiers) {
    delete typeSpecifier;
  }
  mTypeSpecifiers.clear();
}

IObjectType* LLVMStructDefinition::prototypeObject(IRGenerationContext& context) const {
  StructType* structType = context.getModule()->getTypeByName(mName);
  assert(structType == NULL);
  
  structType = StructType::create(context.getLLVMContext(), mName);
  LLVMStructType* llvmStructType = new LLVMStructType(structType);
  context.addLLVMStructType(llvmStructType);

  return NULL;
}

void LLVMStructDefinition::prototypeMethods(IRGenerationContext& context) const {
  LLVMStructType* llvmStructType = context.getLLVMStructType(mName);
  StructType* structType = llvmStructType->getLLVMType(context);
  assert(structType);
  
  vector<Type*> types;
  for (const ILLVMTypeSpecifier* llvmTypeSpecifier : mTypeSpecifiers) {
    const IType* type = llvmTypeSpecifier->getType(context);
    types.push_back(type->getLLVMType(context));
  }
  structType->setBody(types);
}

Value* LLVMStructDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}
