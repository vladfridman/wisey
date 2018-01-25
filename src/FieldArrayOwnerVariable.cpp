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

const ArrayOwnerType* FieldArrayOwnerVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->getTypeKind() == ARRAY_OWNER_TYPE);
  
  return (const ArrayOwnerType*) type;
}

Value* FieldArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
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
  Field* field = checkAndFindFieldForAssignment(context, mObject, mName);
  
  const IType* fieldType = field->getType();
  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, assignToType, assignToValue, fieldType, line);
  
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  Value* fieldPointerLoaded = IRWriter::newLoadInst(context, fieldPointer, "");
  
  ((IObjectOwnerType*) field->getType())->free(context, fieldPointerLoaded);

  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

Value* FieldArrayOwnerVariable::generateArrayElementAssignment(IRGenerationContext& context,
                                                               IExpression* assignToExpression,
                                                               vector<const IExpression*>
                                                               arrayIndices,
                                                               int line) {
  Field* field = checkAndFindFieldForAssignment(context, mObject, mName);

  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  Value* arrayPointer = IRWriter::newLoadInst(context, fieldPointer, "");
  const IType* fieldType = field->getType();
  assert(fieldType->getTypeKind() == ARRAY_OWNER_TYPE);
  const ArrayOwnerType* arrayOwnerType = (const ArrayOwnerType*) fieldType;
  const IType* elementType = arrayOwnerType->getArrayType()->getElementType();
  Value* elementStore = ArrayElementExpression::generateElementIR(context,
                                                                  arrayOwnerType->getArrayType(),
                                                                  arrayPointer,
                                                                  arrayIndices);
  
  return ArrayElementAssignment::generateElementAssignment(context,
                                                           elementType,
                                                           assignToExpression,
                                                           elementStore,
                                                           line);
}

void FieldArrayOwnerVariable::setToNull(IRGenerationContext& context) {
  PointerType* type = (PointerType*) getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  IRWriter::newStoreInst(context, null, fieldPointer);
}

void FieldArrayOwnerVariable::free(IRGenerationContext& context) const {
  /** Freed using object destructor */
}

