//
//  FixedFieldDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/FixedField.hpp"
#include "wisey/FixedFieldDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

FixedFieldDefinition::FixedFieldDefinition(const ITypeSpecifier* typeSpecifier,
                                           string name,
                                           int line) :
mTypeSpecifier(typeSpecifier), mName(name), mLine(line) { }

FixedFieldDefinition::~FixedFieldDefinition() {
  delete mTypeSpecifier;
}

IField* FixedFieldDefinition::define(IRGenerationContext& context,
                                      const IObjectType* objectType) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  
  return new FixedField(fieldType, mName, mLine);
}

bool FixedFieldDefinition::isConstant() const {
  return false;
}

bool FixedFieldDefinition::isField() const {
  return true;
}

bool FixedFieldDefinition::isMethod() const {
  return false;
}

bool FixedFieldDefinition::isStaticMethod() const {
  return false;
}

bool FixedFieldDefinition::isMethodSignature() const {
  return false;
}

bool FixedFieldDefinition::isLLVMFunction() const {
  return false;
}

