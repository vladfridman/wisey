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
  delete mControllerTypeSpecifier;
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
  string fullName = mControllerTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  Controller* controller = new Controller(fullName, structType);
  context.addController(controller);
}

void ControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerTypeSpecifier->getName(context));

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
  vector<Field*> receivedFields = createReceivedFields(context,
                                                       mReceivedFieldDeclarations,
                                                       offset);
  vector<Field*> injectedFields = createInjectedFields(context,
                                                       mInjectedFieldDeclarations,
                                                       offset + receivedFields.size());
  vector<Field*> stateFields = createStateFields(context,
                                                 mStateFieldDeclarations,
                                                 offset + receivedFields.size()
                                                 + injectedFields.size());
  vector<Field*> allFields;
  allFields.insert(allFields.end(), receivedFields.begin(), receivedFields.end());
  allFields.insert(allFields.end(), injectedFields.begin(), injectedFields.end());
  allFields.insert(allFields.end(), stateFields.begin(), stateFields.end());

  controller->setFields(allFields);

  createFieldVariables(context, controller, types);
  controller->setStructBodyTypes(types);

  IConcreteObjectType::generateNameGlobal(context, controller);
  IConcreteObjectType::generateVTable(context, controller);
}

Value* ControllerDefinition::generateIR(IRGenerationContext& context) const {
  Controller* controller = context.getController(mControllerTypeSpecifier->getName(context));
  
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

vector<Field*> ControllerDefinition::createReceivedFields(IRGenerationContext& context,
                                                          vector<FieldDeclaration*> declarations,
                                                          unsigned long startIndex) const {
  vector<Field*> fields;
  for (FieldDeclaration* fieldDeclaration : declarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    Field* field = new Field(FieldKind::RECEIVED_FIELD,
                             fieldType,
                             fieldDeclaration->getName(),
                             startIndex + fields.size(),
                             fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}

vector<Field*> ControllerDefinition::createInjectedFields(IRGenerationContext& context,
                                                          vector<FieldDeclaration*> declarations,
                                                          unsigned long startIndex) const {
  vector<Field*> fields;
  for (FieldDeclaration* fieldDeclaration : declarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    if (fieldType->getTypeKind() == INTERFACE_OWNER_TYPE) {
      Interface* interface = (Interface*) ((IObjectOwnerType*) fieldType)->getObject();
      fieldType = context.getBoundController(interface)->getOwner();
    }

    Field* field = new Field(FieldKind::INJECTED_FIELD,
                             fieldType,
                             fieldDeclaration->getName(),
                             startIndex + fields.size(),
                             fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}

vector<Field*> ControllerDefinition::createStateFields(IRGenerationContext& context,
                                                       vector<FieldDeclaration*> declarations,
                                                       unsigned long startIndex) const {
  vector<Field*> fields;
  for (FieldDeclaration* fieldDeclaration : declarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    
    Field* field = new Field(FieldKind::STATE_FIELD,
                             fieldType,
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

    if (fieldDeclaration->getFieldKind() == INJECTED_FIELD &&
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

