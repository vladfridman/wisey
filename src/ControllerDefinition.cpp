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
  mReceivedFeildDeclarations.clear();
  mInjectedFeildDeclarations.clear();
  mStateFeildDeclarations.clear();
  mMethodDeclarations.clear();
}

Value* ControllerDefinition::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  StructType *structType = StructType::create(llvmContext, "controller." + mName);
  
  vector<Type*> types;
  vector<Interface*> interfaces = processInterfaces(context, types);
  map<string, Field*> fields = createFields(context, interfaces.size());
  vector<Method*> methods = createMethods(context);
  Controller* controller = new Controller(mName, structType, fields, methods, interfaces);
  
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

map<string, Field*> ControllerDefinition::createFields(IRGenerationContext& context,
                                                       unsigned long numberOfInterfaces) const {
  map<string, Field*> fields;
  unsigned long index = 0;
  for (ControllerFieldDeclaration* fieldDeclaration : mReceivedFeildDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier().getType(context);
    
    Field* field = new Field(fieldType, numberOfInterfaces + index);
    fields[fieldDeclaration->getName()] = field;
    index++;
  }
  for (ControllerFieldDeclaration* fieldDeclaration : mInjectedFeildDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier().getType(context);
    
    Field* field = new Field(fieldType, numberOfInterfaces + index);
    fields[fieldDeclaration->getName()] = field;
    index++;
  }
  for (ControllerFieldDeclaration* fieldDeclaration : mStateFeildDeclarations) {
    IType* fieldType = fieldDeclaration->getTypeSpecifier().getType(context);
    
    Field* field = new Field(fieldType, numberOfInterfaces + index);
    fields[fieldDeclaration->getName()] = field;
    index++;
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
