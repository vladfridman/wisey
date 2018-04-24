//
//  FieldImmutableArrayOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FieldImmutableArrayOwnerVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldImmutableArrayOwnerVariable::
FieldImmutableArrayOwnerVariable(string name, const IConcreteObjectType* object) :
mName(name), mObject(object) { }

FieldImmutableArrayOwnerVariable::~FieldImmutableArrayOwnerVariable() {}

string FieldImmutableArrayOwnerVariable::getName() const {
  return mName;
}

bool FieldImmutableArrayOwnerVariable::isField() const {
  return true;
}

bool FieldImmutableArrayOwnerVariable::isSystem() const {
  return false;
}

const ImmutableArrayOwnerType* FieldImmutableArrayOwnerVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isArray() && type->isOwner() && type->isImmutable());
  
  return (const ImmutableArrayOwnerType*) type;
}

Value* FieldImmutableArrayOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldImmutableArrayOwnerVariable::
generateIdentifierReferenceIR(IRGenerationContext& context) const {
  return getFieldPointer(context, mObject, mName);
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

Value* FieldImmutableArrayOwnerVariable::generateWholeArrayAssignment(IRGenerationContext& context,
                                                                      IExpression* assignToExpression,
                                                                      int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName);
  
  const IType* fieldType = field->getType();
  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, assignToType, assignToValue, fieldType, line);
  
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  Value* fieldPointerLoaded = IRWriter::newLoadInst(context, fieldPointer, "");
  
  ((const IOwnerType*) field->getType())->free(context, fieldPointerLoaded, line);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

Value* FieldImmutableArrayOwnerVariable::
generateArrayElementAssignment(IRGenerationContext& context,
                               IExpression* assignToExpression,
                               vector<const IExpression*> arrayIndices,
                               int line) {
  context.reportError(line, "Attempting to change a value in an immutable array");
  exit(1);
}

void FieldImmutableArrayOwnerVariable::setToNull(IRGenerationContext& context) {
  llvm::PointerType* type = (llvm::PointerType*) getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  IRWriter::newStoreInst(context, null, fieldPointer);
}

void FieldImmutableArrayOwnerVariable::free(IRGenerationContext& context, int line) const {
  /** Freed using object destructor */
}
