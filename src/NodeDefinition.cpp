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
  checkFields(context, mFieldDeclarations);

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

void NodeDefinition::checkFields(IRGenerationContext& context,
                                 vector<FieldDeclaration*> fieldDeclarations) {
  for (FieldDeclaration* fieldDeclaration : fieldDeclarations) {
    FieldKind fieldKind = fieldDeclaration->getFieldKind();
    
    if (fieldKind != STATE_FIELD && fieldKind != FIXED_FIELD) {
      Log::e("Nodes can only have fixed or state fields");
      exit(1);
    }
    
    const IType* type = fieldDeclaration->getTypeSpecifier()->getType(context);
    if (fieldKind == STATE_FIELD && type->getTypeKind() != NODE_OWNER_TYPE) {
      Log::e("Node state fields can only be node owner type");
      exit(1);
    }
  }
}
