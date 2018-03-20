//
//  StateFieldDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/StateField.hpp"
#include "wisey/StateFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

StateFieldDeclaration::StateFieldDeclaration(const ITypeSpecifier* typeSpecifier, string name) :
mTypeSpecifier(typeSpecifier), mName(name) { }

StateFieldDeclaration::~StateFieldDeclaration() {
  delete mTypeSpecifier;
}

IField* StateFieldDeclaration::define(IRGenerationContext& context,
                                      const IObjectType* objectType) const {
  const IType* fieldType = mTypeSpecifier->getType(context);
  
  return new StateField(fieldType, mName);
}

bool StateFieldDeclaration::isConstant() const {
  return false;
}

bool StateFieldDeclaration::isField() const {
  return true;
}

bool StateFieldDeclaration::isMethod() const {
  return false;
}

bool StateFieldDeclaration::isStaticMethod() const {
  return false;
}

bool StateFieldDeclaration::isMethodSignature() const {
  return false;
}
