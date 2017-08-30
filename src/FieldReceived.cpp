//
//  FieldReceived.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/FieldReceived.hpp"

using namespace std;
using namespace wisey;

FieldReceived::~FieldReceived() {
  // Argument expressions are deleted with delcarations
  mArguments.clear();
}

const IType* FieldReceived::getType() const {
  return mType;
}

string FieldReceived::getName() const {
  return mName;
}

unsigned long FieldReceived::getIndex() const {
  return mIndex;
}

ExpressionList FieldReceived::getArguments() const {
  return mArguments;
}

bool FieldReceived::isAssignable() const {
  return true;
}
