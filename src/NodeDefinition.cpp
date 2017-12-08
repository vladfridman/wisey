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
  string fullName = IObjectDefinition::getFullName(context, mNodeTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Node* node = Node::newNode(fullName, structType);
  context.addNode(node);
  node->setImportProfile(context.getImportProfile());

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(node);
  IObjectDefinition::prototypeInnerObjects(context, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
}

void NodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mNodeTypeSpecifierFull);
  Node* node = context.getNode(fullName);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(node);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context, node, mObjectElementDeclarations, mInterfaceSpecifiers);
  context.setObjectType(lastObjectType);
}

Value* NodeDefinition::generateIR(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mNodeTypeSpecifierFull);
  Node* node = context.getNode(fullName);
  
  context.getScopes().pushScope();
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(node);

  IObjectDefinition::generateInnerObjectIR(context, mInnerObjectDefinitions);
  IConcreteObjectType::defineCurrentObjectNameVariable(context, node);
  IConcreteObjectType::generateStaticMethodsIR(context, node);
  IConcreteObjectType::declareFieldVariables(context, node);
  IConcreteObjectType::generateMethodsIR(context, node);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, node);
  IConcreteObjectType::composeInterfaceMapFunctions(context, node);

  context.setObjectType(lastObjectType);
  context.getScopes().popScope(context, 0);
  
  return NULL;
}

