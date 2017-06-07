//
//  Field.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Field.hpp"

using namespace std;
using namespace wisey;

Field::~Field() {
  // Argument expressions are deleted with delcarations
  mArguments.clear();
}

const IType* Field::getType() const {
  return mType;
}

string Field::getName() const {
  return mName;
}

unsigned long Field::getIndex() const {
  return mIndex;
}

ExpressionList Field::getArguments() const {
  return mArguments;
}
