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

using namespace llvm;
using namespace std;
using namespace wisey;

ControllerDefinition::~ControllerDefinition() {
  for (FieldDeclaration* fieldDeclaration : mReceivedFieldDeclarations) {
    delete fieldDeclaration;
  }
  mReceivedFieldDeclarations.clear();
  for (FieldDeclaration* fieldDeclaration : mInjectedFieldDeclarations) {
    delete fieldDeclaration;
  }
  mInjectedFieldDeclarations.clear();
  for (FieldDeclaration* fieldDeclaration : mStateFieldDeclarations) {
    delete fieldDeclaration;
  }
  mStateFieldDeclarations.clear();
  for (IMethodDeclaration* methodDeclaration : mMethodDeclarations) {
    delete methodDeclaration;
  }
  mMethodDeclarations.clear();
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ControllerDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = getFullName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Controller* controller = new Controller(fullName, structType);
  context.addController(controller);
}

void ControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  Controller* controller = context.getController(getFullName(context));

  vector<Interface*> interfaces = processInterfaces(context);
  vector<IMethod*> methods = createMethods(context);

  controller->setInterfaces(interfaces);
  controller->setMethods(methods);

  vector<Type*> types;
  for (Interface* interface : controller->getInterfaces()) {
    types.push_back(interface->getLLVMType(context.getLLVMContext())
                    ->getPointerElementType()->getPointerElementType());
  }
  
  // In object struct fields start after vTables for all its interfaces
  unsigned long offset = controller->getInterfaces().size();
  vector<FieldReceived*> receivedFields = createReceivedFields(context,
                                                               mReceivedFieldDeclarations,
                                                               offset);
  vector<FieldInjected*> injectedFields = createInjectedFields(context,
                                                               mInjectedFieldDeclarations,
                                                               offset + receivedFields.size());
  vector<FieldState*> stateFields = createStateFields(context,
                                                      mStateFieldDeclarations,
                                                      offset + receivedFields.size()
                                                      + injectedFields.size());

  controller->setFields(receivedFields, injectedFields, stateFields);

  createFieldVariables(context, controller, types);
  controller->setStructBodyTypes(types);

  IConcreteObjectType::generateNameGlobal(context, controller);
  IConcreteObjectType::generateVTable(context, controller);
}

Value* ControllerDefinition::generateIR(IRGenerationContext& context) const {
  Controller* controller = context.getController(getFullName(context));
  
  context.getScopes().pushScope();
  context.getScopes().setObjectType(controller);
 
  IConcreteObjectType::generateStaticMethodsIR(context, controller);
  IConcreteObjectType::declareFieldVariables(context, controller);
  IConcreteObjectType::composeDestructorBody(context, controller);
  IConcreteObjectType::generateMethodsIR(context, controller);
  
  context.getScopes().popScope(context);
  
  return NULL;
}

vector<Interface*> ControllerDefinition::processInterfaces(IRGenerationContext& context) const {
  vector<Interface*> interfaces;
  for (InterfaceTypeSpecifier* interfaceSpecifier : mInterfaceSpecifiers) {
    Interface* interface = (Interface*) interfaceSpecifier->getType(context);
    interfaces.push_back(interface);
  }
  return interfaces;
}

vector<FieldReceived*> ControllerDefinition::createReceivedFields(IRGenerationContext& context,
                                                                  vector<FieldDeclaration*>
                                                                    declarations,
                                                                  unsigned long startIndex) const {
  vector<FieldReceived*> fields;
  for (FieldDeclaration* fieldDeclaration : declarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    FieldReceived* field = new FieldReceived(fieldType,
                                             fieldDeclaration->getName(),
                                             startIndex + fields.size(),
                                             fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}

vector<FieldInjected*> ControllerDefinition::createInjectedFields(IRGenerationContext& context,
                                                                  vector<FieldDeclaration*>
                                                                  declarations,
                                                                  unsigned long startIndex) const {
  vector<FieldInjected*> fields;
  for (FieldDeclaration* fieldDeclaration : declarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    if (fieldType->getTypeKind() == INTERFACE_OWNER_TYPE) {
      Interface* interface = (Interface*) ((IObjectOwnerType*) fieldType)->getObject();
      fieldType = context.getBoundController(interface)->getOwner();
    }

    FieldInjected* field = new FieldInjected(fieldType,
                                             fieldDeclaration->getName(),
                                             startIndex + fields.size(),
                                             fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}

vector<FieldState*> ControllerDefinition::createStateFields(IRGenerationContext& context,
                                                                  vector<FieldDeclaration*>
                                                                  declarations,
                                                                  unsigned long startIndex) const {
  vector<FieldState*> fields;
  for (FieldDeclaration* fieldDeclaration : declarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    FieldState* field = new FieldState(fieldType,
                                       fieldDeclaration->getName(),
                                       startIndex + fields.size(),
                                       fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}

vector<IMethod*> ControllerDefinition::createMethods(IRGenerationContext& context) const {
  vector<IMethod*> methods;
  for (IMethodDeclaration* methodDeclaration : mMethodDeclarations) {
    IMethod* method = methodDeclaration->createMethod(context);
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
                                                               vector<FieldDeclaration*>
                                                               declarations,
                                                               Controller* controller,
                                                               vector<Type*>& types) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  for (FieldDeclaration* fieldDeclaration : declarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);

    if (fieldDeclaration->getFieldQualifier() == INJECTED_FIELD &&
        fieldType->getTypeKind() == INTERFACE_OWNER_TYPE) {
      Interface* interface = (Interface*) ((IObjectOwnerType*) fieldType)->getObject();
      fieldType = context.getBoundController(interface);
    }
    
    Type* llvmType = fieldType->getLLVMType(llvmContext);
    if (IType::isReferenceType(fieldType)) {
      types.push_back(llvmType->getPointerElementType());
    } else {
      types.push_back(llvmType);
    }
  }
}

string ControllerDefinition::getFullName(IRGenerationContext& context) const {
  return context.getPackage() + "." + mName;
}
