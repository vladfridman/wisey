//
//  FieldFixed.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/FieldFixed.hpp"

using namespace std;
using namespace wisey;

FieldFixed::~FieldFixed() {
  // Argument expressions are deleted with delcarations
  mArguments.clear();
}

const IType* FieldFixed::getType() const {
  return mType;
}

string FieldFixed::getName() const {
  return mName;
}

unsigned long FieldFixed::getIndex() const {
  return mIndex;
}

ExpressionList FieldFixed::getArguments() const {
  return mArguments;
}
