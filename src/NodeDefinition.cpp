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

NodeDefinition::NodeDefinition(NodeTypeSpecifier* nodeTypeSpecifier,
                               vector<FieldDeclaration*> fieldDeclarations,
                               vector<IMethodDeclaration *> methodDeclarations,
                               vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
mNodeTypeSpecifier(nodeTypeSpecifier),
mFieldDeclarations(fieldDeclarations),
mMethodDeclarations(methodDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers) { }

NodeDefinition::~NodeDefinition() {
  delete mNodeTypeSpecifier;
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    delete fieldDeclaration;
  }
  mFieldDeclarations.clear();
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
  string fullName = mNodeTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Node* node = Node::newNode(fullName, structType);
  context.addNode(node);
}

void NodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  Node* node = context.getNode(mNodeTypeSpecifier->getName(context));

  configureObject(context, node, mFieldDeclarations, mMethodDeclarations, mInterfaceSpecifiers);
}

Value* NodeDefinition::generateIR(IRGenerationContext& context) const {
  Node* node = context.getNode(mNodeTypeSpecifier->getName(context));
  
  context.getScopes().pushScope();
  context.getScopes().setObjectType(node);
  
  IConcreteObjectType::defineCurrentObjectNameVariable(context, node);
  IConcreteObjectType::generateStaticMethodsIR(context, node);
  IConcreteObjectType::composeDestructorBody(context, node);
  IConcreteObjectType::declareFieldVariables(context, node);
  IConcreteObjectType::generateMethodsIR(context, node);
  
  context.getScopes().popScope(context);
  
  return NULL;
}

