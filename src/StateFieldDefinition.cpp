//
//  StateFieldDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/StateField.hpp"
#include "wisey/StateFieldDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

StateFieldDefinition::StateFieldDefinition(const ITypeSpecifier* typeSpecifier,
                                           string name,
                                           int line) :
mTypeSpecifier(typeSpecifier), mName(name), mLine(line) { }

StateFieldDefinition::~StateFieldDefinition() {
  delete mTypeSpecifier;
}

IField* StateFieldDefinition::define(IRGenerationContext& context,
                                      const IObjectType* objectType) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  StateField* stateField = new StateField(fieldType, mName, mLine);
  stateField->checkType(context, objectType);
  
  return stateField;
}

bool StateFieldDefinition::isConstant() const {
  return false;
}

bool StateFieldDefinition::isField() const {
  return true;
}

bool StateFieldDefinition::isMethod() const {
  return false;
}

bool StateFieldDefinition::isStaticMethod() const {
  return false;
}

bool StateFieldDefinition::isMethodSignature() const {
  return false;
}

bool StateFieldDefinition::isLLVMFunction() const {
  return false;
}

