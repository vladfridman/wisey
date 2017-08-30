//
//  FieldState.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/FieldState.hpp"

using namespace std;
using namespace wisey;

FieldState::~FieldState() {
  // Argument expressions are deleted with delcarations
  mArguments.clear();
}

const IType* FieldState::getType() const {
  return mType;
}

string FieldState::getName() const {
  return mName;
}

unsigned long FieldState::getIndex() const {
  return mIndex;
}

ExpressionList FieldState::getArguments() const {
  return mArguments;
}

bool FieldState::isAssignable() const {
  return true;
}
