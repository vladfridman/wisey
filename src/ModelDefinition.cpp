//
//  ModelDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalValue.h>
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

  map<string, ModelField*>* fields = new map<string, ModelField*>();
  map<string, Method*>* methods = new map<string, Method*>();
  Model* model = new Model(mName, structType, fields, methods);
  context.getScopes().pushScope();
  
  vector<Type*> types;
  vector<Interface*> interfaces = processInterfaces(context, types);
  processFields(context, model, fields, types, (int) interfaces.size());
  structType->setBody(types);
  vector<Constant*> vtableArray = processMethods(context, model, methods);
  processInterfaceMethods(context, model, interfaces, vtableArray);
  generateVTableIR(context, model, vtableArray);
  
  context.addModel(model);
  
  context.getScopes().popScope(context.getBasicBlock());

  return NULL;
}

void ModelDefinition::processFields(IRGenerationContext& context,
                                    Model* model,
                                    map<string, ModelField*>* fields,
                                    vector<Type*>& types,
                                    int index) const {
  LLVMContext& llvmContext = context.getLLVMContext();

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
}

std::vector<llvm::Constant*> ModelDefinition::processMethods(IRGenerationContext& context,
                                                             Model* model,
                                                             map<string, Method*>* methods) const {
  Type* pointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vector<Constant*> modelMethodsArray;
  
  for (vector<MethodDeclaration *>::iterator iterator = mMethods.begin();
       iterator != mMethods.end();
       iterator++) {
    MethodDeclaration* methodDeclaration = *iterator;
    Method* method = methodDeclaration->getMethod(context);
    (*methods)[method->getName()] = method;
    Function* function = methodDeclaration->generateIR(context, model);
    Constant* bitCast = ConstantExpr::getBitCast(function, pointerType);
    modelMethodsArray.push_back(bitCast);
  }
  return modelMethodsArray;
}

std::vector<Interface*> ModelDefinition::processInterfaces(IRGenerationContext& context,
                                                           vector<Type*>& types) const {
  vector<Interface*> interfaces;
  for (std::vector<std::string>::iterator iterator = mInterfaces.begin();
       iterator != mInterfaces.end();
       iterator++) {
    Interface* interface = context.getInterface(*iterator);
    types.push_back(interface->getLLVMType(context.getLLVMContext()));
    interfaces.push_back(interface);
  }
  return interfaces;
}

void ModelDefinition::processInterfaceMethods(IRGenerationContext& context,
                                              Model* model,
                                              vector<Interface*> interfaces,
                                              vector<Constant*>& vtableArray) const {
  int index = 0;
  for (vector<Interface*>::iterator iterator = interfaces.begin();
       iterator != interfaces.end();
       iterator++, index++) {
    Interface* interface = *iterator;
    interface->generateMapFunctionsIR(context, model, vtableArray, index);
  }
}

void ModelDefinition::generateVTableIR(IRGenerationContext& context,
                                       Model* model,
                                       vector<Constant*>& vtableArray) const {
  Type* pointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  ArrayRef<Constant*> arrayRef(vtableArray);
  ArrayType* arrayType = ArrayType::get(pointerType, vtableArray.size());
  Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
  
  string name = "model." + model->getName() + ".vtable";
  new GlobalVariable(*context.getModule(),
                     arrayType,
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     constantArray,
                     name);
}
