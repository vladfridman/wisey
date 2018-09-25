//
//  FieldOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "AutoCast.hpp"
#include "Composer.hpp"
#include "FieldOwnerVariable.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"

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

bool FieldOwnerVariable::isStatic() const {
  return false;
}

int FieldOwnerVariable::getLine() const {
  return mLine;
}

Value* FieldOwnerVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  IField* field = mObject->findField(mName);

  return field->getValue(context, mObject, fieldPointer, line);
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
  
  Composer::setLineNumber(context, line);

  const IType* expressionType = assignToExpression->getType(context);
  const IType* fieldType = field->getType();
  if (!expressionType->canAutoCastTo(context, fieldType)) {
    context.reportError(line, "Can not assign to field '" + mName + "' of object '" +
                        mObject->getTypeName() + "' because of incompatable types");
    throw 1;
  }
  Value* expressionValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  Value* fieldPointerLoaded = IRWriter::newLoadInst(context, fieldPointer, "");

  ((const IOwnerType*) field->getType())->free(context, fieldPointerLoaded, NULL, NULL, line);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

void FieldOwnerVariable::setToNull(IRGenerationContext& context, int line) {
  IField* field = mObject->findField(mName);
  if (!field->isAssignable(mObject)) {
    context.reportError(line,
                        "Setting an unassignable owner field '" + mName + "' of object " +
                        mObject->getTypeName() + " to null possibly by returning its value");
    throw 1;
  }
  llvm::PointerType* type = (llvm::PointerType*) getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  IRWriter::newStoreInst(context, null, fieldPointer);
}

void FieldOwnerVariable::free(IRGenerationContext& context, Value* exception, int line) const {
  /** Freed using object destructor */
}

