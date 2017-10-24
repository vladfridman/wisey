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

string FieldReferenceVariable::getName() const {
  return mName;
}

const IType* FieldReferenceVariable::getType() const {
  return mObject->findField(mName)->getType();
}

Value* FieldReferenceVariable::getValue() const {
  return mValue;
}

Value* FieldReferenceVariable::generateIdentifierIR(IRGenerationContext& context,
                                                    string llvmVariableName) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  if (getType()->getTypeKind() == PRIMITIVE_TYPE) {
    return IRWriter::newLoadInst(context, fieldPointer, "referenceFieldIdentifier");
  }
  
  return fieldPointer;
}

Value* FieldReferenceVariable::generateAssignmentIR(IRGenerationContext& context,
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

  if (fieldType->getTypeKind() == PRIMITIVE_TYPE) {
    return IRWriter::newStoreInst(context, cast, fieldPointer);
  }
  
  Value* castLoaded = IRWriter::newLoadInst(context, cast, "");
  
  return IRWriter::newStoreInst(context, castLoaded, fieldPointer);
}

void FieldReferenceVariable::setToNull(IRGenerationContext& context) {
}

void FieldReferenceVariable::free(IRGenerationContext& context) const {
  /** Not implmeneted yet */
}

bool FieldReferenceVariable::existsInOuterScope() const {
  return true;
}
