//
//  FixedField.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/FixedField.hpp"

using namespace std;
using namespace wisey;

FixedField::FixedField(const IType* type, string name) :
mType(type), mName(name) { }

FixedField::~FixedField() {
}

const IType* FixedField::getType() const {
  return mType;
}

string FixedField::getName() const {
  return mName;
}

bool FixedField::isAssignable() const {
  return false;
}

bool FixedField::isConstant() const {
  return false;
}

bool FixedField::isField() const {
  return true;
}

bool FixedField::isMethod() const {
  return false;
}

bool FixedField::isStaticMethod() const {
  return false;
}

bool FixedField::isMethodSignature() const {
  return false;
}

bool FixedField::isFixed() const {
  return true;
}

bool FixedField::isInjected() const {
  return false;
}

bool FixedField::isReceived() const {
  return false;
}

bool FixedField::isState() const {
  return false;
}

void FixedField::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "  fixed ";
  mType->printToStream(context, stream);
  stream << " " << getName() << ";" << endl;
}

