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
  
  Node* node = new Node(fullName, structType);
  context.addNode(node);
}

void NodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  Node* node = context.getNode(mNodeTypeSpecifier->getName(context));
  node->setFields(createFields(context, mInterfaceSpecifiers.size()));
  
  configureConcreteObject(context,
                          node,
                          mFieldDeclarations,
                          mMethodDeclarations,
                          mInterfaceSpecifiers);
}

Value* NodeDefinition::generateIR(IRGenerationContext& context) const {
  Node* node = context.getNode(mNodeTypeSpecifier->getName(context));
  
  context.getScopes().pushScope();
  context.getScopes().setObjectType(node);
  
  IConcreteObjectType::generateStaticMethodsIR(context, node);
  IConcreteObjectType::composeDestructorBody(context, node);
  IConcreteObjectType::declareFieldVariables(context, node);
  IConcreteObjectType::generateMethodsIR(context, node);
  
  context.getScopes().popScope(context);
  
  return NULL;
}

vector<Field*> NodeDefinition::createFields(IRGenerationContext& context,
                                            unsigned long startIndex) const {
  vector<Field*> fields;
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    const IType* type = fieldDeclaration->getTypeSpecifier()->getType(context);
    FieldKind fieldKind = fieldDeclaration->getFieldKind();

    if (fieldKind != STATE_FIELD && fieldKind != FIXED_FIELD) {
      Log::e("Nodes can only have fixed or state fields");
      exit(1);
    }
    
    if (fieldKind == STATE_FIELD && type->getTypeKind() != NODE_OWNER_TYPE) {
      Log::e("Node state fields can only be node owner type");
      exit(1);
    }
    
    Field* field = new Field(fieldKind,
                             type,
                             fieldDeclaration->getName(),
                             startIndex + fields.size(),
                             fieldDeclaration->getArguments());
    fields.push_back(field);
  }
  
  return fields;
}
