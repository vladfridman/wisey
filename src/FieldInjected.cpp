//
//  FieldInjected.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/FieldInjected.hpp"

using namespace std;
using namespace wisey;

FieldInjected::~FieldInjected() {
  // Argument expressions are deleted with delcarations
  mArguments.clear();
}

const IType* FieldInjected::getType() const {
  return mType;
}

string FieldInjected::getName() const {
  return mName;
}

unsigned long FieldInjected::getIndex() const {
  return mIndex;
}

ExpressionList FieldInjected::getArguments() const {
  return mArguments;
}

bool FieldInjected::isAssignable() const {
  return false;
}

void FieldInjected::extractHeader(iostream& stream) const {
  stream << "  inject " << getType()->getName() << " " << getName();
  if (!mArguments.size()) {
    stream << "endl";
    return;
  }
  
  stream << "(";
  for (IExpression* expression : mArguments) {
    expression->printToStream(stream);
    if (expression != mArguments.at(mArguments.size() - 1)) {
      stream << ", ";
    }
  }
  stream << ");" << endl;
}
