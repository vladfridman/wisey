//
//  FieldImmutableArrayOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "AutoCast.hpp"
#include "ArrayElementAssignment.hpp"
#include "ArrayElementExpression.hpp"
#include "Composer.hpp"
#include "FieldImmutableArrayOwnerVariable.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldImmutableArrayOwnerVariable::
FieldImmutableArrayOwnerVariable(string name, const IConcreteObjectType* object, int line) :
mName(name), mObject(object), mLine(line) { }

FieldImmutableArrayOwnerVariable::~FieldImmutableArrayOwnerVariable() {}

string FieldImmutableArrayOwnerVariable::getName() const {
  return mName;
}

bool FieldImmutableArrayOwnerVariable::isField() const {
  return true;
}

bool FieldImmutableArrayOwnerVariable::isStatic() const {
  return false;
}

int FieldImmutableArrayOwnerVariable::getLine() const {
  return mLine;
}

const ImmutableArrayOwnerType* FieldImmutableArrayOwnerVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isArray() && type->isOwner() && type->isImmutable());
  
  return (const ImmutableArrayOwnerType*) type;
}

Value* FieldImmutableArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context,
                                                              int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldImmutableArrayOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                                       int line) const {
  return getFieldPointer(context, mObject, mName, line);
}

Value* FieldImmutableArrayOwnerVariable::
generateAssignmentIR(IRGenerationContext& context,
                     IExpression* assignToExpression,
                     vector<const IExpression*> arrayIndices,
                     int line) {
  Composer::setLineNumber(context, line);

  Value* result = arrayIndices.size()
  ? generateArrayElementAssignment(context, assignToExpression, arrayIndices, line)
  : generateWholeArrayAssignment(context, assignToExpression, line);
  
  return result;
}

Value* FieldImmutableArrayOwnerVariable::
generateWholeArrayAssignment(IRGenerationContext& context,
                             IExpression* assignToExpression,
                             int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName, line);
  
  const IType* fieldType = field->getType();
  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, assignToType, assignToValue, fieldType, line);
  
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  Value* fieldPointerLoaded = IRWriter::newLoadInst(context, fieldPointer, "");
  
  ((const IOwnerType*) field->getType())->free(context, fieldPointerLoaded, NULL, NULL, line);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

Value* FieldImmutableArrayOwnerVariable::
generateArrayElementAssignment(IRGenerationContext& context,
                               IExpression* assignToExpression,
                               vector<const IExpression*> arrayIndices,
                               int line) {
  context.reportError(line, "Attempting to change a value in an immutable array");
  throw 1;
}

void FieldImmutableArrayOwnerVariable::setToNull(IRGenerationContext& context, int line) {
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

void FieldImmutableArrayOwnerVariable::free(IRGenerationContext& context,
                                            Value* exception,
                                            int line) const {
  /** Freed using object destructor */
}
