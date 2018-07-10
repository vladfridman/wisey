//
//  FieldArrayReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/ArrayElementAssignment.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/Composer.hpp"
#include "wisey/FieldArrayReferenceVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldArrayReferenceVariable::FieldArrayReferenceVariable(string name,
                                                         const IConcreteObjectType* object,
                                                         int line) :
mName(name), mObject(object), mLine(line) { }

FieldArrayReferenceVariable::~FieldArrayReferenceVariable() {}

string FieldArrayReferenceVariable::getName() const {
  return mName;
}

bool FieldArrayReferenceVariable::isField() const {
  return true;
}

bool FieldArrayReferenceVariable::isStatic() const {
  return false;
}

int FieldArrayReferenceVariable::getLine() const {
  return mLine;
}

const wisey::ArrayType* FieldArrayReferenceVariable::getType() const {
  const IType* type = mObject->findField(mName)->getType();
  assert(type->isArray() && type->isReference());
  
  return (const wisey::ArrayType*) type;
}

Value* FieldArrayReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                         int line) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldArrayReferenceVariable::generateIdentifierReferenceIR(IRGenerationContext& context,
                                                                  int line) const {
  return getFieldPointer(context, mObject, mName, line);
}

Value* FieldArrayReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
                                                     IExpression* assignToExpression,
                                                     vector<const IExpression*> arrayIndices,
                                                     int line) {
  Composer::setLineNumber(context, line);

  Value* result = arrayIndices.size()
  ? generateArrayElementAssignment(context, assignToExpression, arrayIndices, line)
  : generateWholeArrayAssignment(context, assignToExpression, line);
  
  return result;
}

Value* FieldArrayReferenceVariable::generateWholeArrayAssignment(IRGenerationContext& context,
                                                                 IExpression* assignToExpression,
                                                                 int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName, line);
  
  const IType* fieldType = field->getType();
  assert(fieldType->isArray() && fieldType->isReference() && !fieldType->isImmutable());
  const ArrayType* arrayType = (const wisey::ArrayType*) fieldType;
  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, field->getType());
  Value* cast = AutoCast::maybeCast(context, assignToType, assignToValue, fieldType, line);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);

  Value* previousValue = IRWriter::newLoadInst(context, fieldPointer, "");
  arrayType->decrementReferenceCount(context, previousValue);
  arrayType->incrementReferenceCount(context, cast);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

Value* FieldArrayReferenceVariable::generateArrayElementAssignment(IRGenerationContext& context,
                                                                   IExpression* assignToExpression,
                                                                   vector<const IExpression*>
                                                                   arrayIndices,
                                                                   int line) {
  IField* field = checkAndFindFieldForAssignment(context, mObject, mName, line);
  
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName, line);
  Value* arrayPointer = IRWriter::newLoadInst(context, fieldPointer, "");
  const IType* fieldType = field->getType();
  assert(fieldType->isArray() && fieldType->isReference());
  const ArrayType* arrayType = (const wisey::ArrayType*) fieldType;
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

void FieldArrayReferenceVariable::decrementReferenceCounter(IRGenerationContext &context) const {
  /** Decremented using object destructor */
}


