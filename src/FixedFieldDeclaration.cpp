//
//  FixedFieldDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/FixedFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

FixedFieldDeclaration::FixedFieldDeclaration(ITypeSpecifier* typeSpecifier, string name) :
mTypeSpecifier(typeSpecifier), mName(name) { }

FixedFieldDeclaration::~FixedFieldDeclaration() {
  delete mTypeSpecifier;
}

Field* FixedFieldDeclaration::declare(IRGenerationContext& context,
                                      const IObjectType* objectType) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  InjectionArgumentList injectionArgumentList;
  
  return new Field(FIXED_FIELD, fieldType, NULL, mName, injectionArgumentList);
}

bool FixedFieldDeclaration::isConstant() const {
  return false;
}

bool FixedFieldDeclaration::isField() const {
  return true;
}

bool FixedFieldDeclaration::isMethod() const {
  return false;
}

bool FixedFieldDeclaration::isStaticMethod() const {
  return false;
}

bool FixedFieldDeclaration::isMethodSignature() const {
  return false;
}

