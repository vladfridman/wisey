//
//  ExternalNodeDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalNodeDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ExternalNodeDefinition::~ExternalNodeDefinition() {
  delete mNodeTypeSpecifier;
  for (FieldDeclaration* fieldDeclaration : mFixedFieldDeclarations) {
    delete fieldDeclaration;
  }
  mFixedFieldDeclarations.clear();
  for (FieldDeclaration* fieldDeclaration : mStateFieldDeclarations) {
    delete fieldDeclaration;
  }
  mStateFieldDeclarations.clear();
  for (MethodSignatureDeclaration* methodSignature : mMethodSignatures) {
    delete methodSignature;
  }
  mMethodSignatures.clear();
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ExternalNodeDefinition::prototypeObjects(IRGenerationContext& context) const {
  // TODO: implement this
}

void ExternalNodeDefinition::prototypeMethods(IRGenerationContext& context) const {
  // TODO: implement this
}

Value* ExternalNodeDefinition::generateIR(IRGenerationContext& context) const {
  // TODO: implement this
  return NULL;
}

