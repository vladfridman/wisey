//
//  FieldArrayOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FieldArrayOwnerVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldArrayOwnerVariable::FieldArrayOwnerVariable(string name,
                                                 const IConcreteObjectType* object,
                                                 int line) :
mName(name), mObject(object), mLine(line) { }

FieldArrayOwnerVariable::~FieldArrayOwnerVariable() {}

string FieldArrayOwnerVariable::getName() const {
  return mName;
}

bool FieldArrayOwnerVariable::isField() const {
  return true;
}

bool FieldArrayOwnerVariable::isStatic() const {
  return false;
}

int FieldArrayOwnerVariable::getLine() const {
  return mLine;
}

const ArrayOwnerType* FieldArrayOwnerVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isArray() && type->isOwner());
  
  return (const ArrayOwnerType*) type;
}

Value* FieldArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  IField* field = mObject->findField(mName);

  return field->getValue(context, mObject, fieldPointer, line);
}

Value* FieldArrayOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                              int line) const {
  return getFieldPointer(context, mObject, mName, line);
}

Value* FieldArrayOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                     IExpression* assignToExpression,
                                                     vector<const IExpression*> arrayIndices,
                                                     int line) {
  Composer::setLineNumber(context, line);

  Value* result = arrayIndices.size()
  ? generateArrayElementAssignment(context, assignToExpression, arrayIndices, line)
  : generateWholeArrayAssignment(context, assignToExpression, line);
  
  return result;
}

Value* FieldArrayOwnerVariable::generateWholeArrayAssignment(IRGenerationContext& context,
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

Value* FieldArrayOwnerVariable::generateArrayElementAssignment(IRGenerationContext& context,
                                                               IExpression* assignToExpression,
                                                               vector<const IExpression*>
                                                               arrayIndices,
                                                               int line) {
  IField* field = checkAndFindField(context, mObject, mName, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  Value* arrayPointer = field->getValue(context, mObject, fieldPointer, line);

  const IType* fieldType = field->getType();
  assert(fieldType->isArray() && fieldType->isOwner() && !fieldType->isImmutable());
  const ArrayOwnerType* arrayOwnerType = (const ArrayOwnerType*) fieldType;
  const ArrayType* arrayType = arrayOwnerType->getArrayType(context, line);
  const IType* elementType = arrayType->getElementType();
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  arrayType,
                                                                  arrayPointer,
                                                                  arrayIndices,
                                                                  line);
  
  return ArrayElementAssignment::generateElementAssignment(context,
                                                           elementType,
                                                           assignToExpression,
                                                           elementStore,
                                                           line);
}

void FieldArrayOwnerVariable::setToNull(IRGenerationContext& context, int line) {
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

void FieldArrayOwnerVariable::free(IRGenerationContext& context, Value* exception, int line) const {
  /** Freed using object destructor */
}
