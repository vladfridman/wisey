//
//  ReceivedField.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ReceivedField::ReceivedField(const IType* type, string name, int line) :
mType(type), mName(name), mLine(line) { }

ReceivedField::~ReceivedField() {
}

void ReceivedField::checkType(IRGenerationContext& context) const {
  if (!mType->isPrimitive() && !mType->isModel() && !mType->isInterface() &&
      !mType->isArray()) {
    context.reportError(mLine,
                        "Model receive fields can only be of primitive, model or array type");
    throw 1;
  }
  if (mType->isArray() && !mType->isImmutable()) {
    context.reportError(mLine, "Model receive array fields can only be of immutable array type");
    throw 1;
  }
}

const IType* ReceivedField::getType() const {
  return mType;
}

Value* ReceivedField::getValue(IRGenerationContext& context,
                               const IConcreteObjectType* object,
                               Value* fieldPointer,
                               int line) const {
  return IRWriter::newLoadInst(context, fieldPointer, mName);
}

string ReceivedField::getName() const {
  return mName;
}

int ReceivedField::getLine() const {
  return mLine;
}

bool ReceivedField::isAssignable(const IConcreteObjectType* object) const {
  return object->isController();
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

