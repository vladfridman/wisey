//
//  ReceivedFieldDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ReceivedField.hpp"
#include "wisey/ReceivedFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ReceivedFieldDeclaration::ReceivedFieldDeclaration(ITypeSpecifier* typeSpecifier, string name) :
mTypeSpecifier(typeSpecifier), mName(name) { }

ReceivedFieldDeclaration::~ReceivedFieldDeclaration() {
  delete mTypeSpecifier;
}

IField* ReceivedFieldDeclaration::declare(IRGenerationContext& context,
                                      const IObjectType* objectType) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  
  return new ReceivedField(fieldType, mName);
}

bool ReceivedFieldDeclaration::isConstant() const {
  return false;
}

bool ReceivedFieldDeclaration::isField() const {
  return true;
}

bool ReceivedFieldDeclaration::isMethod() const {
  return false;
}

bool ReceivedFieldDeclaration::isStaticMethod() const {
  return false;
}

bool ReceivedFieldDeclaration::isMethodSignature() const {
  return false;
}

