//
//  StateField.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/StateField.hpp"

using namespace std;
using namespace wisey;

StateField::StateField(const IType* type, string name, int line) :
mType(type), mName(name), mLine(line) { }

StateField::~StateField() {
}

const IType* StateField::getType() const {
  return mType;
}

string StateField::getName() const {
  return mName;
}

int StateField::getLine() const {
  return mLine;
}

bool StateField::isAssignable() const {
  return true;
}

bool StateField::isConstant() const {
  return false;
}

bool StateField::isField() const {
  return true;
}

bool StateField::isMethod() const {
  return false;
}

bool StateField::isStaticMethod() const {
  return false;
}

bool StateField::isMethodSignature() const {
  return false;
}

bool StateField::isLLVMFunction() const {
  return false;
}

bool StateField::isFixed() const {
  return false;
}

bool StateField::isInjected() const {
  return false;
}

bool StateField::isReceived() const {
  return false;
}

bool StateField::isState() const {
  return true;
}

void StateField::printToStream(IRGenerationContext& context, iostream& stream) const {
  stream << "  state ";
  mType->printToStream(context, stream);
  stream << " " << getName() << ";" << endl;
}
