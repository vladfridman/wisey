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

NodeDefinition::NodeDefinition(AccessLevel accessLevel,
                               NodeTypeSpecifierFull* nodeTypeSpecifierFull,
                               vector<IObjectElementDefinition*> objectElementDeclarations,
                               vector<IInterfaceTypeSpecifier*> interfaceSpecifiers,
                               vector<IObjectDefinition*> innerObjectDefinitions,
                               bool isPooled,
                               int line) :
mAccessLevel(accessLevel),
mNodeTypeSpecifierFull(nodeTypeSpecifierFull),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers),
mInnerObjectDefinitions(innerObjectDefinitions),
mIsPooled(isPooled),
mLine(line) { }

NodeDefinition::~NodeDefinition() {
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

Node* NodeDefinition::prototypeObject(IRGenerationContext& context,
                                      ImportProfile* importProfile) const {
  string fullName = IObjectDefinition::getFullName(context, mNodeTypeSpecifierFull);
  StructType* structType = StructType::create(context.getLLVMContext(), fullName);
  
  Node* node = mIsPooled
  ? Node::newPooledNode(mAccessLevel, fullName, structType, importProfile, mLine)
  : Node::newNode(mAccessLevel, fullName, structType, importProfile, mLine);
  context.addNode(node, mLine);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(node);
  IObjectDefinition::prototypeInnerObjects(context, importProfile, node, mInnerObjectDefinitions);
  context.setObjectType(lastObjectType);
  
  return node;
}

void NodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mNodeTypeSpecifierFull);
  Node* node = context.getNode(fullName, mLine);

  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(node);
  IObjectDefinition::prototypeInnerObjectMethods(context, mInnerObjectDefinitions);
  configureObject(context, node, mObjectElementDeclarations, mInterfaceSpecifiers, NULL);
  if (node->isPooled()) {
    node->defineAllocateFunction(context);
  } else {
    node->defineBuildFunction(context);
  }
  context.setObjectType(lastObjectType);
}

void NodeDefinition::generateIR(IRGenerationContext& context) const {
  string fullName = IObjectDefinition::getFullName(context, mNodeTypeSpecifierFull);
  Node* node = context.getNode(fullName, mLine);
  
  context.getScopes().pushScope();
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(node);

  IObjectDefinition::generateInnerObjectIR(context, mInnerObjectDefinitions);
  IConcreteObjectType::generateStaticMethodsIR(context, node);
  IConcreteObjectType::declareFieldVariables(context, node);
  IConcreteObjectType::generateMethodsIR(context, node);
  IConcreteObjectType::generateLLVMFunctionsIR(context, node);
  IConcreteObjectType::scheduleDestructorBodyComposition(context, node);
  IConcreteObjectType::composeInterfaceMapFunctions(context, node);

  context.setObjectType(lastObjectType);
  context.getScopes().popScope(context, 0);
}

