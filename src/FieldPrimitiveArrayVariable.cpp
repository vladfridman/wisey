//
//  FieldPrimitiveArrayVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/ArrayElementExpression.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/FieldPrimitiveArrayVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldPrimitiveArrayVariable::FieldPrimitiveArrayVariable(string name,
                                                         const IConcreteObjectType* object) :
mName(name),
mObject(object) { }

FieldPrimitiveArrayVariable::~FieldPrimitiveArrayVariable() {}

string FieldPrimitiveArrayVariable::getName() const {
  return mName;
}

const IType* FieldPrimitiveArrayVariable::getType() const {
  return mObject->findField(mName)->getType();
}

Value* FieldPrimitiveArrayVariable::generateIdentifierIR(IRGenerationContext& context) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return fieldPointer;
}

Value* FieldPrimitiveArrayVariable::generateAssignmentIR(IRGenerationContext& context,
                                                         IExpression* assignToExpression,
                                                         vector<const IExpression*> arrayIndices,
                                                         int line) {
  Field* field = checkAndFindFieldForAssignment(context, mObject, mName);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);

  Value* element = ArrayElementExpression::generateElementIR(context,
                                                             (const ArrayType*) field->getType(),
                                                             fieldPointer,
                                                             arrayIndices);
  const IType* elementType = ((const wisey::ArrayType*) field->getType())->getElementType();

  const IType* assignToType = assignToExpression->getType(context);
  Value* assignToValue = assignToExpression->generateIR(context, field->getType());
  if (!assignToType->canAutoCastTo(elementType)) {
    Log::e("Can not assign to field '" + mName + "' of object '" + mObject->getTypeName() +
           "' because of incompatable types");
    exit(1);
  }
  
  Value* newValue = AutoCast::maybeCast(context, assignToType, assignToValue, elementType, line);
  IRWriter::newStoreInst(context, newValue, element);
  
  return newValue;
}

