//
//  ConstantDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ConstantDefinition.hpp"

using namespace std;
using namespace wisey;

ConstantDefinition::ConstantDefinition(const AccessLevel accessLevel,
                                         const ITypeSpecifier* typeSpecifier,
                                         std::string name,
                                         IExpression* expression) :
mAccessLevel(accessLevel),
mTypeSpecifier(typeSpecifier),
mName(name),
mExpression(expression) { }

ConstantDefinition::~ConstantDefinition() {
  delete mTypeSpecifier;
  delete mExpression;
}

wisey::Constant* ConstantDefinition::define(IRGenerationContext& context,
                                             const IObjectType* objectType) const {
  return new Constant(mAccessLevel, mTypeSpecifier->getType(context), mName, mExpression);
}

bool ConstantDefinition::isConstant() const {
  return true;
}

bool ConstantDefinition::isField() const {
  return false;
}

bool ConstantDefinition::isMethod() const {
  return false;
}

bool ConstantDefinition::isStaticMethod() const {
  return false;
}

bool ConstantDefinition::isMethodSignature() const {
  return false;
}
