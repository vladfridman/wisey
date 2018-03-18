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
  return NULL;
}

void LLVMStructDefinition::prototypeMethods(IRGenerationContext &context) const {
}

Value* LLVMStructDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}
