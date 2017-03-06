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
  map<string, Function*> methodFunctionMap = processMethods(context, model, methods);
  processInterfaceMethods(context, model, interfaces, methodFunctionMap);
  
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

  for (ModelFieldDeclaration* field : mFields) {
    IType* fieldType = field->getTypeSpecifier().getType(context);
    
    ModelField* modelField = new ModelField(fieldType, index);
    (*fields)[field->getName()] = modelField;
    types.push_back(fieldType->getLLVMType(llvmContext));
    ObjectFieldVariable* fieldVariable = new ObjectFieldVariable(field->getName(), NULL, model);
    context.getScopes().setVariable(fieldVariable);
    index++;
  }
}

map<string, Function*> ModelDefinition::processMethods(IRGenerationContext& context,
                                                       Model* model,
                                                       map<string, Method*>* methods) const {
  map<string, Function*> methodFunctionMap;
  
  for (MethodDeclaration* methodDeclaration : mMethods) {
    Method* method = methodDeclaration->getMethod(context);
    (*methods)[method->getName()] = method;
    Function* function = methodDeclaration->generateIR(context, model);
    methodFunctionMap[method->getName()] = function;
  }
  return methodFunctionMap;
}

std::vector<Interface*> ModelDefinition::processInterfaces(IRGenerationContext& context,
                                                           vector<Type*>& types) const {
  vector<Interface*> interfaces;
  for (string interfaceName : mInterfaces) {
    Interface* interface = context.getInterface(interfaceName);
    types.push_back(interface->getLLVMType(context.getLLVMContext()));
    interfaces.push_back(interface);
  }
  return interfaces;
}

void ModelDefinition::processInterfaceMethods(IRGenerationContext& context,
                                              Model* model,
                                              vector<Interface*> interfaces,
                                              map<string, Function*>& methodFunctionMap) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  vector<Constant*> vTableArray;
  vector<Type*> vTableTypes;
  int index = 0;
  for (Interface* interface : interfaces) {
    vector<Constant*> vTablePortion =
      interface->generateMapFunctionsIR(context, model, methodFunctionMap, index);
    ArrayRef<Constant*> arrayRef(vTablePortion);
    Type* pointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
    ArrayType* arrayType = ArrayType::get(pointerType, vTablePortion.size());
    Constant* constantArray = ConstantArray::get(arrayType, arrayRef);

    vTableArray.push_back(constantArray);
    vTableTypes.push_back(arrayType);
    index++;
  }

  StructType* structType = StructType::get(llvmContext, vTableTypes);
  Constant* constantStruct = ConstantStruct::get(structType, vTableArray);
  
  new GlobalVariable(*context.getModule(),
                     structType,
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     constantStruct,
                     model->getVTableName());
}
