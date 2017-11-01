//
//  FieldPrimitiveVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/FieldPrimitiveVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

FieldPrimitiveVariable::FieldPrimitiveVariable(string name,
                                               Value* value,
                                               const IConcreteObjectType* object) :
mName(name),
mValue(value),
mObject(object) { }

FieldPrimitiveVariable::~FieldPrimitiveVariable() {}

string FieldPrimitiveVariable::getName() const {
  return mName;
}

const IType* FieldPrimitiveVariable::getType() const {
  return mObject->findField(mName)->getType();
}

Value* FieldPrimitiveVariable::getValue() const {
  return mValue;
}

Value* FieldPrimitiveVariable::generateIdentifierIR(IRGenerationContext& context,
                                                    string llvmVariableName) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* FieldPrimitiveVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression) {
  Field* field = checkAndFindFieldForAssignment(context, mObject, mName);

  const IType* expressionType = assignToExpression->getType(context);
  const IType* fieldType = field->getType();
  if (!expressionType->canAutoCastTo(fieldType)) {
    Log::e("Can not assign to field '" + mName + "' of object '" + mObject->getName() +
           "' because of incompatable types");
    exit(1);
  }
  Value* expressionValue = assignToExpression->generateIR(context);
  Value* cast = AutoCast::maybeCast(context, expressionType, expressionValue, fieldType);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

void FieldPrimitiveVariable::setToNull(IRGenerationContext& context) {
}

void FieldPrimitiveVariable::free(IRGenerationContext& context) const {
}

bool FieldPrimitiveVariable::existsInOuterScope() const {
  return true;
}
