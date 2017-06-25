//
//  NodeDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/NodeDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

NodeDefinition::~NodeDefinition() {
  for (FieldDeclaration* fieldDeclaration : mFixedFieldDeclarations) {
    delete fieldDeclaration;
  }
  mFixedFieldDeclarations.clear();
  for (FieldDeclaration* fieldDeclaration : mStateFieldDeclarations) {
    delete fieldDeclaration;
  }
  mStateFieldDeclarations.clear();
  for (MethodDeclaration* methodDeclaration : mMethodDeclarations) {
    delete methodDeclaration;
  }
  mMethodDeclarations.clear();
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void NodeDefinition::prototypeObjects(IRGenerationContext& context) const { }

void NodeDefinition::prototypeMethods(IRGenerationContext& context) const { }

Value* NodeDefinition::generateIR(IRGenerationContext& context) const {
  return NULL;
}
