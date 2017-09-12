//
//  NodeDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Log.hpp"
#include "wisey/NodeDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NodeDefinition::~NodeDefinition() {
  for (FieldDeclaration* fieldDeclaration : mFixedFieldDeclarations) {
    delete fieldDeclaration;
  }
  mFixedFieldDeclarations.clear();
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

void NodeDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = getFullName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Node* node = new Node(fullName, structType);
  context.addNode(node);
}

void NodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  Node* node = context.getNode(getFullName(context));
  vector<Interface*> interfaces = processInterfaces(context);
  vector<Method*> methods = createMethods(context);
  node->setMethods(methods);
  node->setInterfaces(interfaces);
  
  // In object struct fields start after vTables for all its interfaces
  unsigned long offset = node->getInterfaces().size();
  vector<FieldFixed*> fixedFields = createFixedFields(context, mFixedFieldDeclarations, offset);
  vector<FieldState*> stateFields = createStateFields(context,
                                                      mStateFieldDeclarations,
                                                      offset + fixedFields.size());
  node->setFields(fixedFields, stateFields);
  
  vector<Type*> types;
  for (Interface* interface : node->getInterfaces()) {
    types.push_back(interface->getLLVMType(context.getLLVMContext())
                    ->getPointerElementType()->getPointerElementType());
  }
  
  createFieldVariables(context, node, types);
  node->setStructBodyTypes(types);
  
  IConcreteObjectType::generateNameGlobal(context, node);
  IConcreteObjectType::generateVTable(context, node);
}

Value* NodeDefinition::generateIR(IRGenerationContext& context) const {
  Node* node = context.getNode(getFullName(context));
  
  context.getScopes().pushScope();
  
  IConcreteObjectType::composeDestructorBody(context, node);
  IConcreteObjectType::declareFieldVariables(context, node);
  
  for (Method* method : node->getMethods()) {
    method->generateIR(context, node);
  }
  
  context.getScopes().popScope(context);
  
  return NULL;
}

string NodeDefinition::getFullName(IRGenerationContext& context) const {
  return context.getPackage() + "." + mName;
}

vector<Interface*> NodeDefinition::processInterfaces(IRGenerationContext& context) const {
  vector<Interface*> interfaces;
  for (InterfaceTypeSpecifier* interfaceSpecifier : mInterfaceSpecifiers) {
    Interface* interface = (Interface*) interfaceSpecifier->getType(context);
    interfaces.push_back(interface);
  }
  return interfaces;
}

vector<Method*> NodeDefinition::createMethods(IRGenerationContext& context) const {
  vector<Method*> methods;
  for (IMethodDeclaration* methodDeclaration : mMethodDeclarations) {
    Method* method = methodDeclaration->createMethod(context);
    methods.push_back(method);
  }
  return methods;
}

vector<FieldFixed*> NodeDefinition::createFixedFields(IRGenerationContext& context,
                                                      vector<FieldDeclaration*> declarations,
                                                      unsigned long startIndex) const {
  vector<FieldFixed*> fields;
  for (FieldDeclaration* fieldDeclaration : declarations) {
    FieldFixed* field = new FieldFixed(fieldDeclaration->getTypeSpecifier()->getType(context),
                                       fieldDeclaration->getName(),
                                       startIndex + fields.size(),
                                       fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}

vector<FieldState*> NodeDefinition::createStateFields(IRGenerationContext& context,
                                                      vector<FieldDeclaration*> declarations,
                                                      unsigned long startIndex) const {
  vector<FieldState*> fields;
  for (FieldDeclaration* fieldDeclaration : declarations) {
    const IType* type = fieldDeclaration->getTypeSpecifier()->getType(context);
    if (type->getTypeKind() != NODE_OWNER_TYPE) {
      Log::e("Node state fields can only be node owner type");
      exit(1);
    }
    FieldState* field = new FieldState(fieldDeclaration->getTypeSpecifier()->getType(context),
                                       fieldDeclaration->getName(),
                                       startIndex + fields.size(),
                                       fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}

void NodeDefinition::createFieldVariables(IRGenerationContext& context,
                                          Node* node,
                                          vector<Type*>& types) const {
  createFieldVariablesForDeclarations(context, mFixedFieldDeclarations, node, types);
  createFieldVariablesForDeclarations(context, mStateFieldDeclarations, node, types);
}

void NodeDefinition::createFieldVariablesForDeclarations(IRGenerationContext& context,
                                                         vector<FieldDeclaration*>
                                                         declarations,
                                                         Node* node,
                                                         vector<Type*>& types) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  for (FieldDeclaration* fieldDeclaration : declarations) {
    const IType* fieldType = fieldDeclaration->getTypeSpecifier()->getType(context);
    Type* llvmType = fieldType->getLLVMType(llvmContext);
    if (IType::isReferenceType(fieldType)) {
      types.push_back(llvmType->getPointerElementType());
    } else {
      types.push_back(llvmType);
    }
  }
}
