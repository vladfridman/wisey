//
//  ExternalConstantDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ExternalConstantDeclaration.hpp"

using namespace std;
using namespace wisey;

ExternalConstantDeclaration::ExternalConstantDeclaration(const ITypeSpecifier* typeSpecifier,
                                                         std::string name,
                                                         int line) :
mTypeSpecifier(typeSpecifier),
mName(name),
mLine(line) { }

ExternalConstantDeclaration::~ExternalConstantDeclaration() {
  delete mTypeSpecifier;
}

wisey::Constant* ExternalConstantDeclaration::define(IRGenerationContext& context,
                                                     const IObjectType* objectType) const {
  return new Constant(PUBLIC_ACCESS, mTypeSpecifier->getType(context), mName, NULL, mLine);
}

bool ExternalConstantDeclaration::isConstant() const {
  return true;
}

bool ExternalConstantDeclaration::isField() const {
  return false;
}

bool ExternalConstantDeclaration::isMethod() const {
  return false;
}

bool ExternalConstantDeclaration::isStaticMethod() const {
  return false;
}

bool ExternalConstantDeclaration::isMethodSignature() const {
  return false;
}

bool ExternalConstantDeclaration::isLLVMFunction() const {
  return false;
}

