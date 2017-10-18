//
//  ExternalNodeDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalNodeDefinition.hpp"
#include "wisey/NodeDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalNodeDefinition::ExternalNodeDefinition(NodeTypeSpecifier* nodeTypeSpecifier,
                                               vector<FieldDeclaration*> fieldDeclarations,
                                               vector<IMethodDeclaration *> methodDeclarations,
                                               vector<InterfaceTypeSpecifier*>
                                               interfaceSpecifiers) :
mNodeTypeSpecifier(nodeTypeSpecifier),
mFieldDeclarations(fieldDeclarations),
mMethodDeclarations(methodDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers) { }

ExternalNodeDefinition::~ExternalNodeDefinition() {
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

void ExternalNodeDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mNodeTypeSpecifier->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Node* node = Node::newExternalNode(fullName, structType);
  context.addNode(node);
}

void ExternalNodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  Node* node = context.getNode(mNodeTypeSpecifier->getName(context));
  
  configureObject(context, node, mFieldDeclarations, mMethodDeclarations, mInterfaceSpecifiers);
}

Value* ExternalNodeDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

