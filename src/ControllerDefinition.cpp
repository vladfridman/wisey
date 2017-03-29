//
//  ControllerDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Controller.hpp"
#include "yazyk/ControllerDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

ControllerDefinition::~ControllerDefinition() {
  mReceivedFieldDeclarations.clear();
  mInjectedFieldDeclarations.clear();
  mStateFieldDeclarations.clear();
  mMethodDeclarations.clear();
}

Value* ControllerDefinition::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  StructType *structType = StructType::create(llvmContext, "controller." + mName);
  
  vector<Type*> types;
  vector<Interface*> interfaces = processInterfaces(context, types);
  vector<Field*> receivedFields = fieldDeclarationsToFields(context,
                                                            mReceivedFieldDeclarations,
                                                            interfaces.size());
  vector<Field*> injectedFields = fieldDeclarationsToFields(context,
                                                            mInjectedFieldDeclarations,
                                                            interfaces.size() +
                                                            receivedFields.size());
  vector<Field*> stateFields = fieldDeclarationsToFields(context,
                                                         mStateFieldDeclarations,
                                                         interfaces.size() + receivedFields.size() +
                                                         injectedFields.size());
  vector<Method*> methods = createMethods(context);
  Controller* controller = new Controller(mName,
                                          structType,
                                          receivedFields,
                                          injectedFields,
                                          stateFields,
                                          methods,
                                          interfaces);
  
  context.getScopes().pushScope();

  context.addController(controller);
  context.getScopes().popScope(context);
  
  return NULL;
}

vector<Interface*> ControllerDefinition::processInterfaces(IRGenerationContext& context,
                                                           vector<Type*>& types) const {
  vector<Interface*> interfaces;
  for (string interfaceName : mInterfaces) {
    Interface* interface = context.getInterface(interfaceName);
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
  for (ControllerFieldDeclaration* fieldDeclaration : mReceivedFieldDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier().getType(context);
    
    Field* field = new Field(fieldType, fieldDeclaration->getName(), startIndex + fields.size());
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
