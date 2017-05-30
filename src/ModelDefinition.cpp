//
//  ModelDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/DerivedTypes.h>

#include "wisey/Environment.hpp"
#include "wisey/LocalHeapVariable.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/ObjectFieldVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ModelDefinition::~ModelDefinition() {
  mFieldDeclarations.clear();
  mMethodDeclarations.clear();
}

void ModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = getFullName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Model* model = new Model(fullName, structType);
  context.addModel(model);
}

Value* ModelDefinition::generateIR(IRGenerationContext& context) const {
  vector<Type*> types;
  vector<Interface*> interfaces = processInterfaces(context, types);
  map<string, Field*> fields = createFields(context, interfaces.size());
  vector<Method*> methods = createMethods(context);
  Model* model = context.getModel(getFullName(context));
  model->setFields(fields);
  model->setMethods(methods);
  model->setInterfaces(interfaces);

  context.getScopes().pushScope();

  createFieldVariables(context, model, types);
  model->setStructBodyTypes(types);
  map<string, Function*> methodFunctionMap = generateMethodsIR(context, model);
  defineTypeName(context, model);
  GlobalVariable* typeListGlobal = createTypeListGlobal(context, model);
  
  vector<vector<Constant*>> vTables;
  addTypeListInfo(context, vTables, typeListGlobal);
  addUnthunkInfo(context, model, vTables);
  generateInterfaceMapFunctions(context, model, vTables, interfaces, methodFunctionMap);
  createVTableGlobal(context, model, vTables);
  
  context.addImport(model);
  context.getScopes().popScope(context);

  model->createRTTI(context);
  
  return NULL;
}

map<string, Field*> ModelDefinition::createFields(IRGenerationContext& context,
                                                  unsigned long numberOfInterfaces) const {
  map<string, Field*> fields;
  ExpressionList arguments;
  for (ModelFieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    Field* field = new Field(fieldType,
                             fieldDeclaration->getName(),
                             numberOfInterfaces + fields.size(),
                             arguments);
    fields[fieldDeclaration->getName()] = field;
  }
  return fields;
}

void ModelDefinition::createFieldVariables(IRGenerationContext& context,
                                           Model* model,
                                           vector<Type*>& types) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  for (ModelFieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    types.push_back(fieldType->getLLVMType(llvmContext));
    ObjectFieldVariable* fieldVariable = new ObjectFieldVariable(fieldDeclaration->getName(),
                                                                 NULL,
                                                                 model);
    context.getScopes().setVariable(fieldVariable);
  }
}

vector<Method*> ModelDefinition::createMethods(IRGenerationContext& context) const {
  vector<Method*> methods;
  for (MethodDeclaration* methodDeclaration : mMethodDeclarations) {
    Method* method = methodDeclaration->createMethod(context, methods.size());
    methods.push_back(method);
  }
  return methods;
}

map<string, Function*> ModelDefinition::generateMethodsIR(IRGenerationContext& context,
                                                          Model* model) const {
  map<string, Function*> methodFunctionMap;
  vector<tuple<Method*, Function*>> methodsWithFunctions;
  
  for (MethodDeclaration* methodDeclaration : mMethodDeclarations) {
    Method* method = model->findMethod(methodDeclaration->getMethodName());
    Function* function = method->defineFunction(context, model);
    methodFunctionMap[method->getName()] = function;
    methodsWithFunctions.push_back(make_tuple(method, function));
  }
  
  for (tuple<Method*, Function*> methodAndFunction : methodsWithFunctions) {
    Method* method = get<0>(methodAndFunction);
    Function* function = get<1>(methodAndFunction);
    method->generateIR(context, function, model);
  }
  
  return methodFunctionMap;
}

std::vector<Interface*> ModelDefinition::processInterfaces(IRGenerationContext& context,
                                                           vector<Type*>& types) const {
  vector<Interface*> interfaces;
  for (InterfaceTypeSpecifier* interfaceSpecifier : mInterfaceSpecifiers) {
    Interface* interface = dynamic_cast<Interface*>(interfaceSpecifier->getType(context));
    types.push_back(interface->getLLVMType(context.getLLVMContext())->getPointerElementType());
    interfaces.push_back(interface);
  }
  return interfaces;
}

void ModelDefinition::addTypeListInfo(IRGenerationContext& context,
                                      vector<vector<Constant*>>& vTables,
                                      GlobalVariable* typeListGlobal) const {
  Type* pointerType = Type::getInt8Ty(context.getLLVMContext())->getPointerTo();
  vector<Constant*> vTablePortion;
  
  Constant* bitCast = ConstantExpr::getBitCast(typeListGlobal, pointerType);
  vTablePortion.push_back(ConstantExpr::getNullValue(pointerType));
  vTablePortion.push_back(bitCast);

  vTables.push_back(vTablePortion);
}


void ModelDefinition::addUnthunkInfo(IRGenerationContext& context,
                                     Model* model,
                                     vector<vector<Constant*>>& vTables) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  unsigned long vTableSize = model->getFlattenedInterfaceHierarchy().size();
  
  for (unsigned long i = 1; i < vTableSize; i++) {
    vector<Constant*> vTablePortion;
    
    long unthunkBy = -Environment::getAddressSizeInBytes() * i;
    ConstantInt* unthunk = ConstantInt::get(Type::getInt64Ty(llvmContext), unthunkBy);
    vTablePortion.push_back(ConstantExpr::getIntToPtr(unthunk, pointerType));
    vTablePortion.push_back(ConstantExpr::getNullValue(pointerType));
    
    vTables.push_back(vTablePortion);
  }
}

void ModelDefinition::generateInterfaceMapFunctions(IRGenerationContext& context,
                                                    Model* model,
                                                    vector<vector<Constant*>>& vTables,
                                                    vector<Interface*> interfaces,
                                                    map<string, Function*>&
                                                    methodFunctionMap) const {
  
  vector<list<Constant*>> interfaceMapFunctions;
  for (Interface* interface : interfaces) {
    vector<list<Constant*>> vSubTable =
    interface->generateMapFunctionsIR(context,
                                      model,
                                      methodFunctionMap,
                                      interfaceMapFunctions.size());
    for (list<Constant*> vTablePortion : vSubTable) {
      interfaceMapFunctions.push_back(vTablePortion);
    }
  }
  
  assert(interfaceMapFunctions.size() == 0 || interfaceMapFunctions.size() == vTables.size());
  
  int vTablesIndex = 0;
  for (list<Constant*> interfaceMapFunctionsPortion : interfaceMapFunctions) {
    for (Constant* constant : interfaceMapFunctionsPortion) {
      vTables.at(vTablesIndex).push_back(constant);
    }
    vTablesIndex++;
  }
}

void ModelDefinition::createVTableGlobal(IRGenerationContext& context,
                                         Model* model,
                                         vector<vector<Constant*>> interfaceVTables) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  
  vector<Constant*> vTableArray;
  vector<Type*> vTableTypes;
  for (vector<Constant*> vTablePortionVector : interfaceVTables) {
    ArrayRef<Constant*> arrayRef(vTablePortionVector);
    ArrayType* arrayType = ArrayType::get(pointerType, vTablePortionVector.size());
    Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
    
    vTableArray.push_back(constantArray);
    vTableTypes.push_back(arrayType);
  }
  
  StructType* vTableGlobalType = StructType::get(llvmContext, vTableTypes);
  Constant* vTableGlobalConstantStruct = ConstantStruct::get(vTableGlobalType, vTableArray);
  
  new GlobalVariable(*context.getModule(),
                     vTableGlobalType,
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     vTableGlobalConstantStruct,
                     model->getVTableName());
}

void ModelDefinition::defineTypeName(IRGenerationContext& context, Model* model) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Constant* stringConstant = ConstantDataArray::getString(llvmContext, model->getName());
  new GlobalVariable(*context.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     model->getObjectNameGlobalVariableName());
}

GlobalVariable* ModelDefinition::createTypeListGlobal(IRGenerationContext& context,
                                                      Model* model) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  vector<Interface*> interfaces = model->getFlattenedInterfaceHierarchy();
  Type* pointerType = Type::getInt8Ty(llvmContext)->getPointerTo();

  Constant* modelNamePointer = IObjectType::getObjectNamePointer(model, context);
  
  vector<Constant*> typeNames;
  typeNames.push_back(modelNamePointer);
  
  for (Interface* interface : interfaces) {
    Constant* interfaceNamePointer = IObjectType::getObjectNamePointer(interface, context);
    typeNames.push_back(interfaceNamePointer);
  }
  typeNames.push_back(ConstantExpr::getNullValue(pointerType));
  ArrayRef<Constant*> arrayRef(typeNames);
  ArrayType* arrayType = ArrayType::get(pointerType, typeNames.size());
  Constant* constantArray = ConstantArray::get(arrayType, arrayRef);
 
  return new GlobalVariable(*context.getModule(),
                            arrayType,
                            true,
                            GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                            constantArray,
                            model->getTypeTableName());
}

string ModelDefinition::getFullName(IRGenerationContext& context) const {
  return context.getPackage() + "." + mName;
}
