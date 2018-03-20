//
//  ConstantDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ConstantDeclaration.hpp"

using namespace std;
using namespace wisey;

ConstantDeclaration::ConstantDeclaration(const AccessLevel accessLevel,
                                         const ITypeSpecifier* typeSpecifier,
                                         std::string name,
                                         IExpression* expression) :
mAccessLevel(accessLevel),
mTypeSpecifier(typeSpecifier),
mName(name),
mExpression(expression) { }

ConstantDeclaration::~ConstantDeclaration() {
  delete mTypeSpecifier;
  delete mExpression;
}

wisey::Constant* ConstantDeclaration::define(IRGenerationContext& context,
                                             const IObjectType* objectType) const {
  return new Constant(mAccessLevel, mTypeSpecifier->getType(context), mName, mExpression);
}

bool ConstantDeclaration::isConstant() const {
  return true;
}

bool ConstantDeclaration::isField() const {
  return false;
}

bool ConstantDeclaration::isMethod() const {
  return false;
}

bool ConstantDeclaration::isStaticMethod() const {
  return false;
}

bool ConstantDeclaration::isMethodSignature() const {
  return false;
}
