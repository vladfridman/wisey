//
//  ExternalModelDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalModelDefinition.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

ExternalModelDefinition::~ExternalModelDefinition() {
  delete mModelTypeSpecifier;
  for (FieldDeclaration* fieldDeclaration : mFieldDeclarations) {
    delete fieldDeclaration;
  }
  mFieldDeclarations.clear();
  for (MethodSignatureDeclaration* methodSignature : mMethodSignatures) {
    delete methodSignature;
  }
  mMethodSignatures.clear();
  for (InterfaceTypeSpecifier* interfaceTypeSpecifier : mInterfaceSpecifiers) {
    delete interfaceTypeSpecifier;
  }
  mInterfaceSpecifiers.clear();
}

void ExternalModelDefinition::prototypeObjects(IRGenerationContext& context) const {
  // TODO: implement this
}

void ExternalModelDefinition::prototypeMethods(IRGenerationContext& context) const {
  // TODO: implement this
}

Value* ExternalModelDefinition::generateIR(IRGenerationContext& context) const {
  // TODO: implement this
  return NULL;
}
