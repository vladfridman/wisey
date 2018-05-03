//
//  FieldOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FieldOwnerVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldOwnerVariable::FieldOwnerVariable(string name, const IConcreteObjectType* object, int line) :
mName(name), mObject(object), mLine(line) { }

FieldOwnerVariable::~FieldOwnerVariable() {}

string FieldOwnerVariable::getName() const {
  return mName;
}

const IOwnerType* FieldOwnerVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isOwner());
  
  return (const IOwnerType*) type;
}

bool FieldOwnerVariable::isField() const {
  return true;
}

bool FieldOwnerVariable::isSystem() const {
  return false;
}

int FieldOwnerVariable::getLine() const {
  return mLine;
}

Value* FieldOwnerVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  IField* field = mObject->findField(mName);

  if (!field->isInjected()) {
    return IRWriter::newLoadInst(context, fieldPointer, "");
  }
  
  assert(mObject->isController() && "Injected field in an object other than controller");
  const Controller* controller  = (const Controller*) mObject;
  return ((InjectedField* ) field)->callInjectFunction(context, controller, fieldPointer);
}

Value* FieldOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                         int line) const {
  return getFieldPointer(context, mObject, mName, line);
}

Value* FieldOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression* assignToExpression,
                                                vector<const IExpression*> arrayIndices,
                                                int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName, line);
  if (field->isInjected()) {
    context.reportError(line,
                        "Attempt to assign to injected field '" + mName + "' of object " +
                        mObject->getTypeName() + ", assignment to injected fields is not allowed");
    exit(1);
  }
  
  Composer::setLineNumber(context, line);

  const IType* expressionType = assignToExpression->getType(context);
  const IType* fieldType = field->getType();
  if (!expressionType->canAutoCastTo(context, fieldType)) {
    context.reportError(line, "Can not assign to field '" + mName + "' of object '" +
                        mObject->getTypeName() + "' because of incompatable types");
    exit(1);
  }
  Value* expressionValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  Value* fieldPointerLoaded = IRWriter::newLoadInst(context, fieldPointer, "");

  ((const IOwnerType*) field->getType())->free(context, fieldPointerLoaded, line);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

void FieldOwnerVariable::setToNull(IRGenerationContext& context, int line) {
  IField* field = mObject->findField(mName);
  if (field->isInjected()) {
    context.reportError(line,
                        "Attempting to set an injected field '" + mName + "' of object " +
                        mObject->getTypeName() + " to null possibly by returning its value");
    exit(1);
  }
  llvm::PointerType* type = (llvm::PointerType*) getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  IRWriter::newStoreInst(context, null, fieldPointer);
}

void FieldOwnerVariable::free(IRGenerationContext& context, int line) const {
  /** Freed using object destructor */
}

