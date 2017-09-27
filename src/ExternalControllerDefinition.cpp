//
//  ExternalControllerDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalControllerDefinition.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ExternalControllerDefinition::~ExternalControllerDefinition() {
  delete mControllerTypeSpecifier;
  for (FieldDeclaration* fieldDeclaration : mReceivedFieldDeclarations) {
    delete fieldDeclaration;
  }
  mReceivedFieldDeclarations.clear();
  for (FieldDeclaration* fieldDeclaration : mInjectedFieldDeclarations) {
    delete fieldDeclaration;
  }
  mInjectedFieldDeclarations.clear();
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

void ExternalControllerDefinition::prototypeObjects(IRGenerationContext& context) const {
  // TODO implement this
}

void ExternalControllerDefinition::prototypeMethods(IRGenerationContext& context) const {
  // TODO implement this
}

Value* ExternalControllerDefinition::generateIR(IRGenerationContext& context) const {
  // TODO implement this
  return NULL;
}
