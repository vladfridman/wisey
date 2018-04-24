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

FieldOwnerVariable::FieldOwnerVariable(string name,
                                       const IConcreteObjectType* object) :
mName(name), mObject(object) { }

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

Value* FieldOwnerVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldOwnerVariable::generateIdentifierReferenceIR(IRGenerationContext& context) const {
  return getFieldPointer(context, mObject, mName);
}

Value* FieldOwnerVariable::generateAssignmentIR(IRGenerationContext& context,
                                                IExpression* assignToExpression,
                                                vector<const IExpression*> arrayIndices,
                                                int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName);

  Composer::setLineNumber(context, line);

  const IType* expressionType = assignToExpression->getType(context);
  const IType* fieldType = field->getType();
  if (!expressionType->canAutoCastTo(context, fieldType)) {
    Log::e_deprecated("Can not assign to field '" + mName + "' of object '" + mObject->getTypeName() +
           "' because of incompatable types");
    exit(1);
  }
  Value* expressionValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  Value* fieldPointerLoaded = IRWriter::newLoadInst(context, fieldPointer, "");

  ((const IOwnerType*) field->getType())->free(context, fieldPointerLoaded, line);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

void FieldOwnerVariable::setToNull(IRGenerationContext& context) {
  llvm::PointerType* type = (llvm::PointerType*) getType()->getLLVMType(context);
  Value* null = ConstantPointerNull::get(type);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  IRWriter::newStoreInst(context, null, fieldPointer);
}

void FieldOwnerVariable::free(IRGenerationContext& context, int line) const {
  /** Freed using object destructor */
}

