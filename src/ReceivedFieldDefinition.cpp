//
//  ReceivedFieldDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ReceivedField.hpp"
#include "wisey/ReceivedFieldDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ReceivedFieldDefinition::ReceivedFieldDefinition(const ITypeSpecifier* typeSpecifier,
                                                   string name) :
mTypeSpecifier(typeSpecifier), mName(name) { }

ReceivedFieldDefinition::~ReceivedFieldDefinition() {
  delete mTypeSpecifier;
}

IField* ReceivedFieldDefinition::define(IRGenerationContext& context,
                                         const IObjectType* objectType) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  
  return new ReceivedField(fieldType, mName);
}

bool ReceivedFieldDefinition::isConstant() const {
  return false;
}

bool ReceivedFieldDefinition::isField() const {
  return true;
}

bool ReceivedFieldDefinition::isMethod() const {
  return false;
}

bool ReceivedFieldDefinition::isStaticMethod() const {
  return false;
}

bool ReceivedFieldDefinition::isMethodSignature() const {
  return false;
}
