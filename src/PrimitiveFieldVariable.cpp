//
//  PrimitiveFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/PrimitiveFieldVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string PrimitiveFieldVariable::getName() const {
  return mName;
}

const IType* PrimitiveFieldVariable::getType() const {
  return mObject->findField(mName)->getType();
}

Value* PrimitiveFieldVariable::getValue() const {
  return mValue;
}

Value* PrimitiveFieldVariable::generateIdentifierIR(IRGenerationContext& context,
                                                    string llvmVariableName) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* PrimitiveFieldVariable::generateAssignmentIR(IRGenerationContext& context,
                                                    IExpression* assignToExpression) {
  if (mObject->getTypeKind() == MODEL_TYPE) {
    Log::e("Can not modify model's fields, models are immutable");
    exit(1);
  }
  
  IField* field = checkAndFindField(context, mObject, mName);
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

void PrimitiveFieldVariable::setToNull(IRGenerationContext& context) const {
}

void PrimitiveFieldVariable::free(IRGenerationContext& context) const {
}

bool PrimitiveFieldVariable::existsInOuterScope() const {
  return true;
}
