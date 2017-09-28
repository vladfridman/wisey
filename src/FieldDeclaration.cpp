//
//  FieldDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/FieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

FieldDeclaration::~FieldDeclaration() {
  delete mTypeSpecifier;
  for (IExpression* expression : mArguments) {
    delete expression;
  }
  mArguments.clear();
}

FieldKind FieldDeclaration::getFieldKind() const {
  return mFieldKind;
}

ITypeSpecifier* FieldDeclaration::getTypeSpecifier() const {
  return mTypeSpecifier;
}

string FieldDeclaration::getName() const {
  return mName;
}

ExpressionList FieldDeclaration::getArguments() const {
  return mArguments;
}
