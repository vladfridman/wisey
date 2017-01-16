//
//  ModelDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/DerivedTypes.h>

#include "yazyk/ModelDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  StructType *structType = StructType::create(llvmContext, mName);

  vector<Type*> types;
  
  for(std::vector<ModelFieldDeclaration *>::iterator iterator = mFields.begin();
      iterator != mFields.end();
      iterator++) {
    ModelFieldDeclaration *field = *iterator;
    types.push_back(field->getType().getType(context));
  }
  
  structType->setBody(types);
  
  context.addModelType(mName, structType);
  
  return NULL;
}
