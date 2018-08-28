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
                                       IExpression* expression,
                                       int line) :
mAccessLevel(accessLevel),
mTypeSpecifier(typeSpecifier),
mName(name),
mExpression(expression),
mLine(line) { }

ConstantDefinition::~ConstantDefinition() {
  delete mTypeSpecifier;
  delete mExpression;
}

wisey::Constant* ConstantDefinition::define(IRGenerationContext& context,
                                            const IObjectType* objectType) const {
  const IType* constantType = mTypeSpecifier->getType(context);
  return new Constant(mAccessLevel, constantType, mName, mExpression, mLine);
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

bool ConstantDefinition::isLLVMFunction() const {
  return false;
}
