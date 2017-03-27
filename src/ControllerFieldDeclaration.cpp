//
//  ControllerFieldDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/ControllerFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

FieldQualifier ControllerFieldDeclaration::getFieldQualifier() const {
  return mFieldQualifier;
}

ITypeSpecifier& ControllerFieldDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

string ControllerFieldDeclaration::getName() const {
  return mName;
}
