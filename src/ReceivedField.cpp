//
//  ReceivedField.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/ReceivedField.hpp"

using namespace std;
using namespace wisey;

ReceivedField::ReceivedField(const IType* type, string name, int line) :
mType(type), mName(name), mLine(line) { }

ReceivedField::~ReceivedField() {
}

const IType* ReceivedField::getType() const {
  return mType;
}

string ReceivedField::getName() const {
  return mName;
}

int ReceivedField::getLine() const {
  return mLine;
}

bool ReceivedField::isAssignable() const {
  return true;
}

bool ReceivedField::isConstant() const {
  return false;
}

bool ReceivedField::isField() const {
  return true;
}

bool ReceivedField::isMethod() const {
  return false;
}

bool ReceivedField::isStaticMethod() const {
  return false;
}

bool ReceivedField::isMethodSignature() const {
  return false;
}

bool ReceivedField::isLLVMFunction() const {
  return false;
}

string ReceivedField::getFieldKind() const {
  return "received";
}

bool ReceivedField::isFixed() const {
  return false;
}

bool ReceivedField::isInjected() const {
  return false;
}

bool ReceivedField::isReceived() const {
  return true;
}

bool ReceivedField::isState() const {
  return false;
}

void ReceivedField::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "  receive " << mType->getTypeName() << " " << getName() << ";" << endl;
}

