//
//  ControllerFieldDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/ControllerFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ControllerFieldDeclaration::~ControllerFieldDeclaration() {
  delete mTypeSpecifier;
}

FieldQualifier ControllerFieldDeclaration::getFieldQualifier() const {
  return mFieldQualifier;
}

ITypeSpecifier* ControllerFieldDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

string ControllerFieldDeclaration::getName() const {
  return mName;
}

ExpressionList ControllerFieldDeclaration::getArguments() const {
  return mArguments;
}
