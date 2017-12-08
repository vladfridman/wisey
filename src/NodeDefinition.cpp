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

NodeDefinition::NodeDefinition(NodeTypeSpecifierFull* nodeTypeSpecifierFull,
                               vector<IObjectElementDeclaration*> objectElementDeclarations,
                               vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                               vector<IObjectDefinition*> innerObjectDefinitions) :
mNodeTypeSpecifierFull(nodeTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions) { }

NodeDefinition::~NodeDefinition() {
  delete mNodeTypeSpecifierFull;
  for (IObjectElementDeclaration* objectElementDeclaration : mObjectElementDeclarations) {
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

void NodeDefinition::prototypeObjects(IRGenerationContext& context) const {
  string fullName = mNodeTypeSpecifierFull->getName(context);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Node* node = Node::newNode(fullName, structType);
  context.addNode(node);
  node->setImportProfile(context.getImportProfile());
}

void NodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  Node* node = context.getNode(mNodeTypeSpecifierFull->getName(context));

  context.setObjectType(node);
  configureObject(context, node, mObjectElementDeclarations, mInterfaceSpecifiers);
  context.setObjectType(NULL);
}

Value* NodeDefinition::generateIR(IRGenerationContext& context) const {
  Node* node = context.getNode(mNodeTypeSpecifierFull->getName(context));
  
  context.getScopes().pushScope();
  context.setObjectType(node);
  
  IConcreteObjectType::defineCurrentObjectNameVariable(context, node);
  IConcreteObjectType::generateStaticMethodsIR(context, node);
  IConcreteObjectType::declareFieldVariables(context, node);
  IConcreteObjectType::generateMethodsIR(context, node);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, node);
  IConcreteObjectType::composeInterfaceMapFunctions(context, node);

  context.setObjectType(NULL);
  context.getScopes().popScope(context, 0);
  
  return NULL;
}

