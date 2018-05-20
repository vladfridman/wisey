//
//  FixedField.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace std;
using namespace wisey;

FixedField::FixedField(const IType* type, string name, int line) :
mType(type), mName(name), mLine(line) { }

FixedField::~FixedField() {
}

void FixedField::checkType(IRGenerationContext& context) const {
  if (!mType->isPrimitive() && !mType->isModel() && !mType->isInterface() &&
      !mType->isArray()) {
    context.reportError(mLine, "Model fixed fields can only be of primitive, model or array type");
    throw 1;
  }
  if (mType->isArray() && !mType->isImmutable()) {
    context.reportError(mLine, "Model fixed array fields can only be of immutable array type");
    throw 1;
  }
}

const IType* FixedField::getType() const {
  return mType;
}

string FixedField::getName() const {
  return mName;
}

int FixedField::getLine() const {
  return mLine;
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

bool FixedField::isLLVMFunction() const {
  return false;
}

string FixedField::getFieldKind() const {
  return "fixed";
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
  stream << "  fixed " << mType->getTypeName() << " " << getName() << ";" << endl;
}

