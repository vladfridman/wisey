//
//  ObjectFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldReferenceVariable::FieldReferenceVariable(string name,
                                               const IConcreteObjectType* object,
                                               int line) :
mName(name), mObject(object), mLine(line) { }

FieldReferenceVariable::~FieldReferenceVariable() {}

string FieldReferenceVariable::getName() const {
  return mName;
}

const IReferenceType* FieldReferenceVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isReference());
  
  return (const IReferenceType*) type;
}

bool FieldReferenceVariable::isField() const {
  return true;
}

bool FieldReferenceVariable::isStatic() const {
  return false;
}

int FieldReferenceVariable::getLine() const {
  return mLine;
}

Value* FieldReferenceVariable::generateIdentifierIR(IRGenerationContext& context, int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);  
  IField* field = mObject->findField(mName);
  return field->getValue(context, mObject, fieldPointer, line);
}

Value* FieldReferenceVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                             int line) const {
  return getFieldPointer(context, mObject, mName, line);
}

Value* FieldReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression,
                                                    vector<const IExpression*> arrayIndices,
                                                    int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName, line);

  const IType* expressionType = assignToExpression->getType(context);
  assert(field->getType()->isReference());
  const IReferenceType* fieldType = (const IReferenceType*) field->getType();
  if (!expressionType->canAutoCastTo(context, fieldType)) {
    context.reportError(line, "Can not assign to field '" + mName + "' of object '" +
                        mObject->getTypeName() + "' because of incompatable types");
    throw 1;
  }
  Value* expressionValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);

  Value* previousValue = IRWriter::newLoadInst(context, fieldPointer, "");
  fieldType->decrementReferenceCount(context, previousValue);
  fieldType->incrementReferenceCount(context, cast);

  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

void FieldReferenceVariable::decrementReferenceCounter(IRGenerationContext& context) const {
  /** Decremented using object destructor */
}
