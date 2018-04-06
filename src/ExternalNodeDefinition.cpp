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
                                               vector<IObjectElementDefinition*>
                                               objectElementDeclarations,
                                               vector<IInterfaceTypeSpecifier*>
                                               interfaceSpecifiers,
                                               vector<IObjectDefinition*> innerObjectDefinitions,
                                               int line) :
mNodeTypeSpecifierFull(nodeTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions),
mLine(line) { }

ExternalNodeDefinition::~ExternalNodeDefinition() {
  delete mNodeTypeSpecifierFull;
  for (IObjectElementDefinition* objectElementDeclaration : mObjectElementDeclarations) {
    delete objectElementDeclaration;
  }
  mObjectElementDeclarations.clear();
  for (IInterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
  for (IObjectDefinition* innerObjectDefinition : mInnerObjectDefinitions) {
    delete innerObjectDefinition;
  }
  mInnerObjectDefinitions.clear();
}

Node* ExternalNodeDefinition::prototypeObject(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mNodeTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Node* node = Node::newExternalNode(fullName, structType);
  context.addNode(node);
  node->setImportProfile(context.getImportProfile());

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(node);
  IObjectDefinition::prototypeInnerObjects(context, node, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return node;
}

void ExternalNodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mNodeTypeSpecifierFull);
  Node* node = context.getNode(fullName, mLine);
  
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(node);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context, node, mObjectElementDeclarations, mInterfaceSpecifiers);
  context.setObjectType(lastObjectType);
}

Value* ExternalNodeDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}

