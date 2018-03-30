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
                                                 const IConcreteObjectType* object) :
mName(name), mObject(object) { }

FieldArrayOwnerVariable::~FieldArrayOwnerVariable() {}

string FieldArrayOwnerVariable::getName() const {
  return mName;
}

bool FieldArrayOwnerVariable::isField() const {
  return true;
}

bool FieldArrayOwnerVariable::isSystem() const {
  return false;
}

const ArrayOwnerType* FieldArrayOwnerVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isArray() && type->isOwner());
  
  return (const ArrayOwnerType*) type;
}

Value* FieldArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldArrayOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context) const {
  return getFieldPointer(context, mObject, mName);
}

Value* FieldArrayOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                     IExpression* assignToExpression,
                                                     vector<const IExpression*> arrayIndices,
                                                     int line) {
  Composer::pushCallStack(context, line);
  
  Value* result = arrayIndices.size()
  ? generateArrayElementAssignment(context, assignToExpression, arrayIndices, line)
  : generateWholeArrayAssignment(context, assignToExpression, line);
  
  Composer::popCallStack(context);
  
  return result;
}

Value* FieldArrayOwnerVariable::generateWholeArrayAssignment(IRGenerationContext& context,
                                                             IExpression* assignToExpression,
                                                             int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName);
  
  const IType* fieldType = field->getType();
  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, assignToType, assignToValue, fieldType, line);
  
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  Value* fieldPointerLoaded = IRWriter::newLoadInst(context, fieldPointer, "");
  
  ((const IOwnerType*) field->getType())->free(context, fieldPointerLoaded);

  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

Value* FieldArrayOwnerVariable::generateArrayElementAssignment(IRGenerationContext& context,
                                                               IExpression* assignToExpression,
                                                               vector<const IExpression*>
                                                               arrayIndices,
                                                               int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName);

  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  Value* arrayPointer = IRWriter::newLoadInst(context, fieldPointer, "");
  const IType* fieldType = field->getType();
  assert(fieldType->isArray() && fieldType->isOwner());
  const ArrayOwnerType* arrayOwnerType = (const ArrayOwnerType*) fieldType;
  const ArrayType* arrayType = arrayOwnerType->getArrayType(context);
  const IType* elementType = arrayType->getElementType();
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  arrayType,
                                                                  arrayPointer,
                                                                  arrayIndices);
  
  return ArrayElementAssignment::generateElementAssignment(context,
                                                           elementType,
                                                           assignToExpression,
                                                           elementStore,
                                                           line);
}

void FieldArrayOwnerVariable::setToNull(IRGenerationContext& context) {
  llvm::PointerType* type = (llvm::PointerType*) getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  IRWriter::newStoreInst(context, null, fieldPointer);
}

void FieldArrayOwnerVariable::free(IRGenerationContext& context) const {
  /** Freed using object destructor */
}
