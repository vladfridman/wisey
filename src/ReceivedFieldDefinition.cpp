//
//  ReceivedFieldDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include "ReceivedField.hpp"
#include "ReceivedFieldDefinition.hpp"
#include "Log.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

ReceivedFieldDefinition::ReceivedFieldDefinition(const ITypeSpecifier* typeSpecifier,
                                                 string name,
                                                 bool isImplied,
                                                 int line) :
mTypeSpecifier(typeSpecifier), mName(name), mIsImplied(isImplied), mLine(line) { }

ReceivedFieldDefinition::~ReceivedFieldDefinition() {
  delete mTypeSpecifier;
}

ReceivedFieldDefinition* ReceivedFieldDefinition::create(const ITypeSpecifier* typeSpecifier,
                                                         string name,
                                                         int line) {
  return new ReceivedFieldDefinition(typeSpecifier, name, false, line);
}

ReceivedFieldDefinition* ReceivedFieldDefinition::createImplied(const ITypeSpecifier* typeSpecifier,
                                                                string name,
                                                                int line) {
  return new ReceivedFieldDefinition(typeSpecifier, name, true, line);
}

IField* ReceivedFieldDefinition::define(IRGenerationContext& context,
                                        const IObjectType* objectType) const {
  if (mIsImplied && !objectType->isModel()) {
    string objectKind = objectType->isController() ? "controllers" : "nodes";
    context.reportError(mLine, "Received field delcarations in " + objectKind +
                        " must have 'receive' keyword preceding the field type");
    throw 1;
  }
  const IType* fieldType = mTypeSpecifier->getType(context);
  ReceivedField* field = new ReceivedField(fieldType, mName, mLine);
  if (objectType->isModel()) {
    field->checkType(context);
  }
  return field;
}

bool ReceivedFieldDefinition::isConstant() const {
  return false;
}

bool ReceivedFieldDefinition::isField() const {
  return true;
}

bool ReceivedFieldDefinition::isMethod() const {
  return false;
}

bool ReceivedFieldDefinition::isStaticMethod() const {
  return false;
}

bool ReceivedFieldDefinition::isMethodSignature() const {
  return false;
}

bool ReceivedFieldDefinition::isLLVMFunction() const {
  return false;
}

