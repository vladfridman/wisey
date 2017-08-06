//
//  OwnerFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Log.hpp"
#include "wisey/OwnerFieldVariable.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

string OwnerFieldVariable::getName() const {
  return mName;
}

const IType* OwnerFieldVariable::getType() const {
  return mObject->findField(mName)->getType();
}

Value* OwnerFieldVariable::getValue() const {
  return mValue;
}

Value* OwnerFieldVariable::generateIdentifierIR(IRGenerationContext& context,
                                                      string llvmVariableName) const {
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  
  return IRWriter::newLoadInst(context, fieldPointer, "");
}

Value* OwnerFieldVariable::generateAssignmentIR(IRGenerationContext& context,
                                                      IExpression* assignToExpression) {
  Field* field = checkAndFindField(context, mObject, mName);
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
  
  Value* objectPointer = IRWriter::newLoadInst(context, fieldPointer, "fieldObjectToFree");
  Composer::freeIfNotNull(context, objectPointer);
  
  return IRWriter::newStoreInst(context, cast, fieldPointer);
}

void OwnerFieldVariable::setToNull(IRGenerationContext& context) const {
  PointerType* type = (PointerType*) getType()->getLLVMType(context.getLLVMContext());
  Value* null = ConstantPointerNull::get(type);
  GetElementPtrInst* fieldPointer = getFieldPointer(context, mObject, mName);
  IRWriter::newStoreInst(context, null, fieldPointer);
}

void OwnerFieldVariable::free(IRGenerationContext& context) const {
  /** Not implmeneted yet */
}

bool OwnerFieldVariable::existsInOuterScope() const {
  return true;
}
