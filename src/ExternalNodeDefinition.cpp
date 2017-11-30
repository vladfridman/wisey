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

ExternalNodeDefinition::ExternalNodeDefinition(NodeTypeSpecifierFull* nodeTypeSpecifierFull,
                                               vector<IObjectElementDeclaration*>
                                               objectElementDeclarations,
                                               vector<IInterfaceTypeSpecifier*>
                                               interfaceSpecifiers) :
mNodeTypeSpecifierFull(nodeTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers) { }

ExternalNodeDefinition::~ExternalNodeDefinition() {
  delete mNodeTypeSpecifierFull;
  for (IObjectElementDeclaration* objectElementDeclaration : mObjectElementDeclarations) {
    delete objectElementDeclaration;
  }
  mObjectElementDeclarations.clear();
  for (IInterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ExternalNodeDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mNodeTypeSpecifierFull->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Node* node = Node::newExternalNode(fullName, structType);
  context.addNode(node);
}

void ExternalNodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  Node* node = context.getNode(mNodeTypeSpecifierFull->getName(context));
  
  context.setObjectType(node);
  configureObject(context, node, mObjectElementDeclarations, mInterfaceSpecifiers);
  context.setObjectType(NULL);
}

Value* ExternalNodeDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

