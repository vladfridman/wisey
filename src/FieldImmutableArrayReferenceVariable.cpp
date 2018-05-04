//
//  FieldImmutableArrayReferenceVariable.cpp
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
#include "wisey/FieldImmutableArrayReferenceVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldImmutableArrayReferenceVariable::
FieldImmutableArrayReferenceVariable(string name, const IConcreteObjectType* object, int line) :
mName(name), mObject(object), mLine(line) { }

FieldImmutableArrayReferenceVariable::~FieldImmutableArrayReferenceVariable() {}

string FieldImmutableArrayReferenceVariable::getName() const {
  return mName;
}

bool FieldImmutableArrayReferenceVariable::isField() const {
  return true;
}

bool FieldImmutableArrayReferenceVariable::isSystem() const {
  return false;
}

int FieldImmutableArrayReferenceVariable::getLine() const {
  return mLine;
}

const ImmutableArrayType* FieldImmutableArrayReferenceVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isArray() && type->isReference() && type->isImmutable());
  
  return (const ImmutableArrayType*) type;
}

Value* FieldImmutableArrayReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                                  int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldImmutableArrayReferenceVariable::
generateIdentifierReferenceIR(IRGenerationContext& context, int line) const {
  return getFieldPointer(context, mObject, mName, line);
}

Value* FieldImmutableArrayReferenceVariable::
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

Value* FieldImmutableArrayReferenceVariable::
generateWholeArrayAssignment(IRGenerationContext& context,
                             IExpression* assignToExpression,
                             int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName, line);

  const IType* fieldType = field->getType();
  assert(fieldType->isArray() && fieldType->isReference() && fieldType->isImmutable());
  const ArrayType* arrayType = (const wisey::ArrayType*) fieldType;
  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, assignToType, assignToValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  Value* previousValue = IRWriter::newLoadInst(context, fieldPointer, "");
  arrayType->decrementReferenceCount(context, previousValue, line);
  arrayType->incrementReferenceCount(context, cast, line);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

Value* FieldImmutableArrayReferenceVariable::
generateArrayElementAssignment(IRGenerationContext& context,
                               IExpression* assignToExpression,
                               vector<const IExpression*> arrayIndices,
                               int line) {
  context.reportError(line, "Attempting to change a value in an immutable array");
  exit(1);
}

void FieldImmutableArrayReferenceVariable::decrementReferenceCounter(IRGenerationContext &context,
                                                                     int line) const {
  /** Decremented using object destructor */
}
