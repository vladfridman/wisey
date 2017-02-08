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
  StructType *structType = StructType::create(llvmContext, "model." + mName);

  vector<Type*> types;
  map<string, ModelField*> *fields = new map<string, ModelField*>();
  
  int index = 0;
  for(std::vector<ModelFieldDeclaration *>::iterator iterator = mFields.begin();
      iterator != mFields.end();
      iterator++, index++) {
    ModelFieldDeclaration *field = *iterator;
    IType* fieldType = field->getTypeSpecifier().getType(context);
    
    ModelField* modelField = new ModelField(fieldType, index);
    (*fields)[field->getName()] = modelField;
    types.push_back(fieldType->getLLVMType(llvmContext));
  }
  
  structType->setBody(types);
  
  Model* model = new Model(mName, structType, fields);
  context.addModel(model);
  
  return NULL;
}
