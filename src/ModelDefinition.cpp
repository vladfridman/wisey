//
//  ModelDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/DerivedTypes.h>

#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/ModelDefinition.hpp"
#include "yazyk/ObjectFieldVariable.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

ModelDefinition::~ModelDefinition() {
  mFields.clear();
  mMethods.clear();
}

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  StructType *structType = StructType::create(llvmContext, "model." + mName);

  vector<Type*> types;
  map<string, ModelField*>* fields = new map<string, ModelField*>();
  map<string, Method*>* methods = new map<string, Method*>();
  Model* model = new Model(mName, structType, fields, methods);
  
  context.getScopes().pushScope();
  
  int index = 0;
  for (vector<ModelFieldDeclaration *>::iterator iterator = mFields.begin();
      iterator != mFields.end();
      iterator++, index++) {
    ModelFieldDeclaration *field = *iterator;
    IType* fieldType = field->getTypeSpecifier().getType(context);
    
    ModelField* modelField = new ModelField(fieldType, index);
    (*fields)[field->getName()] = modelField;
    types.push_back(fieldType->getLLVMType(llvmContext));
    ObjectFieldVariable* fieldVariable = new ObjectFieldVariable(field->getName(), NULL, model);
    context.getScopes().setVariable(fieldVariable);
  }
  
  structType->setBody(types);
  
  for (vector<MethodDeclaration *>::iterator iterator = mMethods.begin();
       iterator != mMethods.end();
       iterator++) {
    MethodDeclaration* methodDeclaration = *iterator;
    Method* method = methodDeclaration->getMethod(context);
    (*methods)[method->getName()] = method;
    methodDeclaration->generateIR(context, model);
  }
  
  context.addModel(model);
  
  context.getScopes().popScope(context.getBasicBlock());

  return NULL;
}
