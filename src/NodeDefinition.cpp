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
                               vector<IObjectElementDeclaration*> objectElementDeclarations,
                               vector<InterfaceTypeSpecifier*> interfaceSpecifiers) :
mNodeTypeSpecifier(nodeTypeSpecifier),
mObjectElementDeclarations(objectElementDeclarations),
mInterfaceSpecifiers(interfaceSpecifiers) { }

NodeDefinition::~NodeDefinition() {
  delete mNodeTypeSpecifier;
  for (IObjectElementDeclaration* objectElementDeclaration : mObjectElementDeclarations) {
    delete objectElementDeclaration;
  }
  mObjectElementDeclarations.clear();
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

  configureObject(context, node, mObjectElementDeclarations, mInterfaceSpecifiers);
}

Value* NodeDefinition::generateIR(IRGenerationContext& context) const {
  Node* node = context.getNode(mNodeTypeSpecifier->getName(context));
  
  context.getScopes().pushScope();
  context.getScopes().setObjectType(node);
  
  IConcreteObjectType::generateConstantsIR(context, node);
  IConcreteObjectType::defineCurrentObjectNameVariable(context, node);
  IConcreteObjectType::generateStaticMethodsIR(context, node);
  IConcreteObjectType::composeDestructorBody(context, node);
  IConcreteObjectType::declareFieldVariables(context, node);
  IConcreteObjectType::generateMethodsIR(context, node);
  
  context.getScopes().popScope(context);
  
  return NULL;
}

