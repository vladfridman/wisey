//
//  ControllerDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Controller.hpp"
#include "wisey/ControllerDefinition.hpp"
#include "wisey/Environment.hpp"
#include "wisey/ObjectFieldVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ControllerDefinition::~ControllerDefinition() {
  mReceivedFieldDeclarations.clear();
  mInjectedFieldDeclarations.clear();
  mStateFieldDeclarations.clear();
  mMethodDeclarations.clear();
}

void ControllerDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = getFullName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Controller* controller = new Controller(fullName, structType);
  context.addController(controller);
}

Value* ControllerDefinition::generateIR(IRGenerationContext& context) const {
  string fullName = getFullName(context);
  Controller* controller = context.getController(fullName);
  
  vector<Type*> types;
  Type* functionType = FunctionType::get(Type::getInt32Ty(context.getLLVMContext()), true);
  Type* arrayOfFunctionsPointerType = functionType->getPointerTo()->getPointerTo();
  types.push_back(arrayOfFunctionsPointerType);

  vector<Interface*> interfaces = processInterfaces(context, types);
  
  // In object struct fields start after vTable for the object and vTables of all othe interfaces
  unsigned long offset = interfaces.size() + 1u;
  vector<Field*> receivedFields = fieldDeclarationsToFields(context,
                                                            mReceivedFieldDeclarations,
                                                            offset);
  vector<Field*> injectedFields = fieldDeclarationsToFields(context,
                                                            mInjectedFieldDeclarations,
                                                            offset +
                                                            receivedFields.size());
  vector<Field*> stateFields = fieldDeclarationsToFields(context,
                                                         mStateFieldDeclarations,
                                                         offset +
                                                         receivedFields.size() +
                                                         injectedFields.size());
  vector<Method*> methods = createMethods(context);

  controller->setFields(receivedFields, injectedFields, stateFields);
  controller->setInterfaces(interfaces);
  controller->setMethods(methods);
  
  context.getScopes().pushScope();

  createFieldVariables(context, controller, types);
  controller->setStructBodyTypes(types);

  IConcreteObjectType::generateNameGlobal(context, controller);
  IConcreteObjectType::generateVTable(context, controller);
  
  context.addImport(controller);
  context.getScopes().popScope(context);
  
  return NULL;
}

vector<Interface*> ControllerDefinition::processInterfaces(IRGenerationContext& context,
                                                           vector<Type*>& types) const {
  vector<Interface*> interfaces;
  for (InterfaceTypeSpecifier* interfaceSpecifier : mInterfaceSpecifiers) {
    Interface* interface = (Interface*) interfaceSpecifier->getType(context);
    types.push_back(interface->getLLVMType(context.getLLVMContext())->getPointerElementType());
    interfaces.push_back(interface);
  }
  return interfaces;
}

vector<Field*> ControllerDefinition::fieldDeclarationsToFields(IRGenerationContext& context,
                                                               vector<ControllerFieldDeclaration*>
                                                                declarations,
                                                               unsigned long startIndex) const {
  vector<Field*> fields;
  for (ControllerFieldDeclaration* fieldDeclaration : declarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    if (fieldDeclaration->getFieldQualifier() == INJECTED_FIELD &&
        fieldType->getTypeKind() == INTERFACE_TYPE) {
      Interface* interface = (Interface*) fieldType;
      fieldType = context.getBoundController(interface);
    }
    
    Field* field = new Field(fieldType,
                             fieldDeclaration->getName(),
                             startIndex + fields.size(),
                             fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}

vector<Method*> ControllerDefinition::createMethods(IRGenerationContext& context) const {
  vector<Method*> methods;
  for (MethodDeclaration* methodDeclaration : mMethodDeclarations) {
    Method* method = methodDeclaration->createMethod(context, methods.size());
    methods.push_back(method);
  }
  return methods;
}

void ControllerDefinition::createFieldVariables(IRGenerationContext& context,
                                                Controller* controller,
                                                vector<Type*>& types) const {
  createFieldVariablesForDeclarations(context, mReceivedFieldDeclarations, controller, types);
  createFieldVariablesForDeclarations(context, mInjectedFieldDeclarations, controller, types);
  createFieldVariablesForDeclarations(context, mStateFieldDeclarations, controller, types);
}

void ControllerDefinition::createFieldVariablesForDeclarations(IRGenerationContext& context,
                                                               vector<ControllerFieldDeclaration*>
                                                               declarations,
                                                               Controller* controller,
                                                               vector<Type*>& types) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  for (ControllerFieldDeclaration* fieldDeclaration : declarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);

    if (fieldDeclaration->getFieldQualifier() == INJECTED_FIELD &&
        fieldType->getTypeKind() == INTERFACE_TYPE) {
      Interface* interface = (Interface*) fieldType;
      fieldType = context.getBoundController(interface);
    }
    
    types.push_back(fieldType->getLLVMType(llvmContext));
    ObjectFieldVariable* fieldVariable = new ObjectFieldVariable(fieldDeclaration->getName(),
                                                                 NULL,
                                                                 controller);
    context.getScopes().setVariable(fieldVariable);
  }
}

string ControllerDefinition::getFullName(IRGenerationContext& context) const {
  return context.getPackage() + "." + mName;
}
